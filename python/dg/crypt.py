import os
import sys
import stat
import random
import datetime
import traceback
from pathlib import Path

cryptmode = 3
magic1 = 0x342b5d
magic2 = 0x89abcdef
finger = 'DG_FILE VER 3.0'
key = '{666B72B0-285C-467B-A9AD-769D405A430C}'
key_ba = bytearray(key, encoding="ascii")
dir_exclude_dir_name = ['pic', 'image', 'tmp', 'temp', 'log', 'build', 'cache', '.git']
dir_exclude_file_suffix = ['.dpj', '.pkg', '.pkgx']
backup_root_dir = Path('d:/work/tmp/DG/Backup')


def dword_from_bytes(buf, i):
    return (buf[i + 3] << 24) + (buf[i + 2] << 16) + (buf[i + 1] << 8) + buf[i]


def dword_to_bytes(buf, i, value):
    buf[i+3] = (value >> 24) & 0xff
    buf[i+2] = (value >> 16) & 0xff
    buf[i+1] = (value >> 8) & 0xff
    buf[i] = value & 0xff


def write_log(log):
    curDir = Path(sys.argv[0]).parent
    logDir = curDir.joinpath('Log')
    if not logDir.exists():
        logDir.mkdir()

    dt = datetime.datetime.now()
    sd = f'{dt.year:04d}-{dt.month:02d}-{dt.day:02d}'
    st = f'{dt.hour:02d}:{dt.minute:02d}:{dt.second:02d}'

    logFile = logDir.joinpath(sd + '.log')
    with open(logFile, 'a+', encoding='utf-8') as f:
        f.writelines(st + '    ' + str(log) + '\n')


def isCrypted(filepath):
    with open(filepath, mode='rb') as f:
        fb = bytearray(f.read())

        if len(fb) < 1024:
            return None

        # 头部
        for i in range(32):
            fb[i] ^= 0x5c
        if fb[15] != cryptmode:
            return None
        if dword_from_bytes(fb, 23) != magic1:
            return None
        if fb[:15].decode('ascii') != finger:
            return None
        if dword_from_bytes(fb, 676) != magic2:
            return None

        # key
        for i in range(128):
            fb[i+32] ^= 0xC5
        if fb[32:70].decode('ascii') != key:
            return None

        # 密钥
        len1 = len(key_ba)
        for i in range(0, 256):
            fb[i+160] ^= key_ba[i % len1]

        return fb


def EncryptFile(ofilepath, nfilepath, bfilepath):
    if not Path(ofilepath).exists():
        write_log(f'file "{ofilepath}" not exist')
        return False

    fb = isCrypted(ofilepath)
    if fb is not None:
        write_log(f'file "{ofilepath}" is crypted')
        return False

    with open(ofilepath, mode='rb') as f:
        fb = bytearray(f.read())
        t = bytearray(1024 + len(fb))

        # 正文复制
        for i in range(len(fb)):
            t[i+1024] = fb[i]

        # 头部 指纹1、模式
        t[0:15] = finger.encode('ascii')
        t[15] = cryptmode
        dword_to_bytes(t, 23, magic1)

        # key
        t[32:70] = key_ba

        # 密钥 0-255
        for i in range(256):
            t[160+i] = i

        # 随机打乱顺序
        for i in range(0):
            j = random.randint(0, 255)
            k = random.randint(0, 255)
            t[160+j], t[160+k] = t[160+k], t[160+j]

        # 指纹2        
        dword_to_bytes(t, 676, magic2)

        # 头部加密
        for i in range(32):
            t[i] ^= 0x5c

        # 正文加密 正文值作索引从密钥中查找密文
        for i in range(len(fb)):
            t[i+1024] = t[160 + t[i+1024]]

        # key加密
        for i in range(128):
            t[i+32] ^= 0xc5

        # 密钥加密
        len1 = len(key_ba)
        for i in range(0, 256):
            t[i+160] ^= key_ba[i % len1]

        # key
        for i in range(32):
            t[i+716] = 0xff

    # 备份文件
    if not bfilepath.parent.exists():
        bfilepath.parent.mkdir(parents=True)
    with open(ofilepath, 'rb') as of:
        with open(bfilepath, 'wb') as bf:
            bf.write(of.read())

    # 保存新文件
    if not nfilepath.parent.exists():
        nfilepath.parent.mkdir(parents=True)
    with open(nfilepath, mode='wb') as nf:
        nf.write(t)

    return True


def DecryptFile(ofilepath, nfilepath, bfilepath):
    if not Path(ofilepath).exists():
        write_log(f'file "{ofilepath}" not exist')
        return False

    fb = isCrypted(ofilepath)
    if fb is None:
        write_log(f'file "{ofilepath}" is not crypted')
        return False

    t = bytearray(len(fb) - 1024)
    k = bytearray(256)
    # 密钥查找表生成 密钥中每个密文在密钥中的索引
    for i in range(256):
        k[fb[i+160]] = i

    # 正文解密 密文在密钥中的位置即为正文
    for i in range(len(t)):
        t[i] = k[fb[i+1024]]

    # 备份文件
    if not bfilepath.parent.exists():
        bfilepath.parent.mkdir(parents=True)
    with open(ofilepath, 'rb') as of:
        with open(bfilepath, 'wb') as bf:
            bf.write(of.read())

    # 保存新文件
    if not nfilepath.parent.exists():
        nfilepath.parent.mkdir(parents=True)
    with open(nfilepath, mode='wb') as nf:
        nf.write(t)

    return True


def CheckDir(dirpath):
    for exclude in dir_exclude_dir_name:
        if str(dirpath).lower().find(exclude) >= 0:
            return False
    for subPath in dirpath.iterdir():
        if subPath.is_file():
            suffix = subPath.suffix
            for exclude in dir_exclude_file_suffix:
                if suffix.lower() == exclude:
                    return False

    return True


def ModifyFile(filepath: Path, mode):    
    try:
        print(filepath)
        
        name = filepath.name[:-len(filepath.suffix)]
        newname = name + '_' + chr(mode) + filepath.suffix
        newpath = filepath.parent.joinpath(newname)

        escPath = str(filepath)
        if escPath[1] == ':':
            escPath = '@' + escPath[0] + '@' + escPath[2:]
        bakpath = backup_root_dir.joinpath(escPath)

        if filepath.exists():
            os.chmod(filepath, stat.S_IREAD | stat.S_IWRITE)
        if bakpath.exists():
            os.chmod(bakpath, stat.S_IREAD | stat.S_IWRITE)
        if newpath.exists():
            os.chmod(newpath, stat.S_IREAD | stat.S_IWRITE)

        if mode == ord('e'):
            res = EncryptFile(filepath, filepath, bakpath)
        elif mode == ord('d'):
            res = DecryptFile(filepath, filepath, bakpath)

        if res:
            write_log('    ' + str(filepath))
            write_log('--->' + str(newpath))
            write_log('--->' + str(bakpath))
    except Exception as e:
        write_log(traceback.format_exc())


def ModifyDir(dirpath: Path, mode):
    if not CheckDir(dirpath):
        return

    for subPath in dirpath.iterdir():
        if subPath.is_dir():
            ModifyDir(subPath, mode)
        elif subPath.is_file():
            ModifyFile(subPath, mode)


if __name__ == '__main__':
    try:
        if False:
            sys.argv.append('-d')
            sys.argv.append('d:/test0')

        if len(sys.argv) != 3:
            exit(-1)

        if sys.argv[1] != '-e' and sys.argv[1] != '-d':
            exit(-2)

        rootPath = Path(sys.argv[2])
        if not rootPath.exists():
            exit(-3)

        write_log(str(sys.argv))

        mode = ord(sys.argv[1][1])
        if rootPath.is_dir():
            ModifyDir(rootPath, mode)
        elif rootPath.is_file():
            ModifyFile(rootPath, mode)
    except Exception as e:
        write_log(traceback.format_exc())
