import random
from pathlib import Path

mode = 3
magic1 = 0x342b5d
magic2 = 0x89abcdef
finger = 'DG_FILE VER 3.0'
key0 = bytearray('Huatu_Config_2014_04_29', encoding="ascii")
key1 = bytearray('{5232C535-350B-4dea-94B1-3FF315F8B6E6}', encoding="ascii")
key2 = bytearray('{6A978658-D900-45ff-A23B-E1A1B4F0342A}', encoding="ascii")
key3 = bytearray('{666B72B0-285C-467B-A9AD-769D405A430C}', encoding="ascii")
key4 = bytearray('{4A254B04-0A0F-4928-92F3-5624CC883C05}', encoding="ascii")
key5 = bytearray('{8C5A9F34-971E-42A2-8BAE-9635D4CCA662}', encoding="ascii")


def hiword(i):
    return i >> 16


def loword(i):
    return i & 0xffff


def byte_word(buf, i):
    return (buf[i + 1] << 8) + buf[i]


def byte_dword(buf, i):
    return (buf[i + 3] << 24) + (buf[i + 2] << 16) + (buf[i + 1] << 8) + buf[i]


def pb(b):
    print(len(b), b)
    l1 = len(b)
    for i in range(0, l1, 4):
        b3 = 0 if i+3 >= l1 else b[i+3]
        b2 = 0 if i+2 >= l1 else b[i+2]
        b1 = 0 if i+1 >= l1 else b[i+1]
        b0 = 0 if i+0 >= l1 else b[i+0]
        print(f'{b3:02x}{b2:02x}{b1:02x}{b0:02x}')


def gen_key0_data(key0):
    # sub_B4D20
    len0 = len(key0)
    keyData = bytearray(258)
    for i in range(256):
        keyData[i] = i

    k = 0
    n = 0

    for i in range(256):
        n += keyData[i] + key0[k]
        n = n % 256
        k = (k + 1) % len0

        keyData[i], keyData[n] = keyData[n], keyData[i]

    return keyData


def f_B4E10(buf, keyData):
    lenb = len(buf)
    v6 = keyData[256]
    v7 = keyData[257]

    for i in range(lenb):
        v6 = (v6 + 1) % 256
        v7 = (v7 + keyData[v6]) % 256

        v4 = keyData[v6]
        keyData[v6] = keyData[v7]
        keyData[v7] = v4

        j = (keyData[v7] + keyData[v6]) % 256
        buf[i] ^= keyData[j]

    keyData[256] = v6
    keyData[257] = v7


def f_F2810(key1):
    len1 = len(key1)
    keyData = bytearray(258)
    for i in range(256):
        keyData[i] = i

    v2 = 0
    v5 = 0
    v6 = 0
    v7 = 0
    v8 = 0
    v10 = 0

    for i in range(256):
        v6 = keyData[i]
        v7 = (v6 + v5 + key1[v2]) & 0x800000ff
        v5 = v7 % 256
        keyData[i] = keyData[v5]
        v8 = v2 + 1
        v10 = v8 % len1
        v2 = v10 % 256
        keyData[v5] = v6

    return keyData


def f_F2880(buf, keyData):
    v2 = keyData[256]
    v3 = keyData[257]

    v5 = 0
    v6 = 0
    v7 = 0
    v8 = 0
    v9 = 0
    v10 = 0
    v11 = 0
    v12 = 0
    v13 = 0
    v14 = 0
    v15 = 0
    v16 = 0

    i = 256
    k = 0

    while True:
        v5 = (v2 + 1) & 0x800000ff
        v6 = v5 % 256
        v5 = keyData[v6]
        v7 = (v3 + v5) % 256
        keyData[v6] = keyData[v7]
        keyData[v7] = v5
        buf[k] ^= keyData[(keyData[v6] + v5) % 256]

        v8 = (v6 + 1) & 0x800000ff
        v9 = v8 % 256
        v8 = keyData[v9]
        v10 = (v7 + v8) % 256
        keyData[v9] = keyData[v10]
        keyData[v10] = v8
        buf[k + 1] ^= keyData[(keyData[v9] + v8) % 256]

        v11 = (v9 + 1) & 0x800000ff
        v12 = v11 % 256
        v11 = keyData[v12]
        v13 = (v10 + v11) % 256
        keyData[v12] = keyData[v13]
        keyData[v13] = v11
        buf[k + 2] ^= keyData[(keyData[v12] + v11) % 256]
        
        v14 = (v12 + 1) % 256
        v2 = v14
        v15 = keyData[v14]
        v16 = (v13 + v15) & 0x800000ff
        v3 = (v13 + v15) % 256
        keyData[v14] = keyData[v16]
        keyData[v16] = v15
        buf[k + 3] ^= keyData[(keyData[v14] + v15) % 256]

        k += 4

        i -= 1
        if i <= 0:
            break

    keyData[256] = v14
    keyData[257] = v15


def res_F0C10(buf):
    t1 = bytearray(36)
    for i in range(15):
        t1[i] = buf[i]
    for i in range(16, 32):
        t1[i] = buf[i - 1]
    
    # sub_129FDD0
    for i in range(32):
        t1[i] ^= 0x5c
    
    # sub_12A0C10
    # t2[:38] == key3
    t2 = bytearray(128)
    for i in range(128):
        t2[i] = buf[i+32] ^ 0xC5

    t3 = bytearray(256)
    for i in range(256):
        t3[i] = buf[160+i]

    # sub_129FE20    
    len1 = len(key3)
    for i in range(0, 256, 4):
        t3[i] ^= key3[i % len1]
        t3[i+1] ^= key3[(i+1) % len1]
        t3[i+2] ^= key3[(i+2) % len1]
        t3[i+3] ^= key3[(i+3) % len1]

    t = bytearray(1024)
    for i in range(36):
        t[i] = t1[i]
    for i in range(128):
        t[36+i] = t2[i]
    for i in range(256):
        t[164+i] = t3[i]
    for i in range(260):
        t[420+i] = buf[416+i]
    for i in range(220):
        t[680+i] = buf[676+i]
    for i in range(124):
        t[900+i] = 0xff
    
    return t, t1, t2, t3


def decrypt_cfg():
    with open('d:/cfg.xml', mode='rb') as f:
        fb = bytearray(f.read())
        
        key0Data = gen_key0_data(key0)
        f_B4E10(fb, key0Data)
        cf = fb.decode('gbk')

        pb(key0)
        pb(key0Data)
        pb(fb)
        print(cf)


def test1():
    with open('d:/aaa', mode='rb') as f:
        fb = bytearray(f.read())

        key1Data = f_F2810(key1)        
        f_F2880(fb, key1Data)
        pb(key1)
        pb(key1Data)
        pb(fb[:1024])


def isCrypted(filepath):
    with open(filepath, mode='rb') as f:
        fb = bytearray(f.read())

        # 头部
        for i in range(32):
            fb[i] ^= 0x5c
        if fb[15] != mode:
            return None
        if byte_dword(fb, 23) != magic1:
            return None
        if fb[:15].decode('ascii') != finger:
            return None
        if byte_dword(fb, 676) != magic2:
            return None

        # key
        for i in range(128):
            fb[i+32] ^= 0xC5
        if fb[32:70].decode('ascii') != key3.decode('ascii'):
            return None

        # 密钥
        len1 = len(key3)
        for i in range(0, 256, 4):
            fb[i+160] ^= key3[i % len1]
            fb[i+161] ^= key3[(i+1) % len1]
            fb[i+162] ^= key3[(i+2) % len1]
            fb[i+163] ^= key3[(i+3) % len1]
        
        return fb


def CryptFile(filepath, nfilepath):
    if not Path(filepath).exists():
        print(f'file {filepath} not exist')
        return

    if isCrypted(filepath) is not None:
        print(f'file {filepath} is crypted')
        return

    with open(filepath, mode='rb') as f:
        fb = bytearray(f.read())
        t = bytearray(1024 + len(fb))
        
        # 正文复制
        for i in range(len(fb)):
            t[i+1024] = fb[i]
        
        # 头部 指纹1、模式
        t[0:15] = finger.encode('ascii')
        t[15] = mode
        t[23] = magic1 & 0xff
        t[24] = (magic1 >> 8) & 0xff
        t[25] = (magic1 >> 16) & 0xff
        t[26] = (magic1 >> 24) & 0xff

        # key
        t[32:70] = key3

        # 密钥 0-255 随机打乱顺序
        for i in range(256):
            t[160+i] = i
        
        for i in range(10):
            j = random.randint(0, 255)
            k = random.randint(0, 255)
            t[160+j], t[160+k] = t[160+k], t[160+j]

        # 指纹2
        t[676] = magic2 & 0xff
        t[677] = (magic2 >> 8) & 0xff
        t[678] = (magic2 >> 16) & 0xff
        t[679] = (magic2 >> 24) & 0xff
        
        # 头部加密
        for i in range(32):
            t[i] ^= 0x5c

        # 正文加密
        for i in range(len(fb)):
            t[i+1024] = t[160 + t[i+1024]]
        
        # key加密
        for i in range(128):
            t[i+32] ^= 0xc5
        
        # 密钥加密
        len1 = len(key3)
        for i in range(0, 256, 4):
            t[i+160] ^= key3[i % len1]
            t[i+161] ^= key3[(i+1) % len1]
            t[i+162] ^= key3[(i+2) % len1]
            t[i+163] ^= key3[(i+3) % len1]

        # key
        for i in range(32):
            t[i+716] = 0xff

    with open(nfilepath, mode='wb') as fn:
        fn.write(t)


def DecryptFile(filepath, nfilepath):
    if not Path(filepath).exists():
        print(f'file {filepath} not exist')
        return

    fb = isCrypted(filepath)
    if fb is None:
        print(f'file {filepath} is not crypted')
        return
    
    t = bytearray(len(fb) - 1024)
    k = bytearray(256)
    # 密钥查找表生成
    for i in range(256):
        k[fb[i+160]] = i

    # 正文解密
    for i in range(len(t)):
        t[i] = k[fb[i+1024]]

    with open(nfilepath, mode='wb') as fn:
        fn.write(t)


if True:
    # DecryptFile('d:/c0.xlsx', 'd:/d1.xlsx')
    # CryptFile('d:/d0.xlsx', 'd:/c1.xlsx')
    test1()
