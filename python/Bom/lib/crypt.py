# -*- coding: utf-8 -*-
"""
@author     : mjsaka
@contact    : qiuxuenan@gmail.com
@time       : 2024/2/6 9:03
@file       : crypt.py
@description: decrypt file
"""

import random
from pathlib import Path


cryptmode = 3
magic1 = 0x342b5d
magic2 = 0x89abcdef
finger = 'DG_FILE VER 3.0'
key = '{666B72B0-285C-467B-A9AD-769D405A430C}'
key_ba = bytearray(key, encoding="ascii")


def dword_from_bytes(buf, i):
    return (buf[i + 3] << 24) + (buf[i + 2] << 16) + (buf[i + 1] << 8) + buf[i]


def dword_to_bytes(buf, i, value):
    buf[i+3] = (value >> 24) & 0xff
    buf[i+2] = (value >> 16) & 0xff
    buf[i+1] = (value >> 8) & 0xff
    buf[i] = value & 0xff


def save(nfilepath, t):
    if not Path(nfilepath).parent.exists():
        nfilepath.parent.mkdir(parents=True)
    
    with open(nfilepath, mode='wb') as nf:
        nf.write(t)


def copy(ofilepath, bfilepath):
    if not Path(bfilepath).parent.exists():
        bfilepath.parent.mkdir(parents=True)
    
    with open(ofilepath, 'rb') as of:
        with open(bfilepath, 'wb') as bf:
            bf.write(of.read())


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


def DecryptFile(ofilepath, nfilepath):
    if not Path(ofilepath).exists():
        return False

    fb = isCrypted(ofilepath)
    if fb is None:
        copy(ofilepath, nfilepath)
        return True

    t = bytearray(len(fb) - 1024)
    k = bytearray(256)
    # 密钥查找表生成 密钥中每个密文在密钥中的索引
    for i in range(256):
        k[fb[i+160]] = i

    # 正文解密 密文在密钥中的位置即为正文
    for i in range(len(t)):
        t[i] = k[fb[i+1024]]

    # 保存新文件
    save(nfilepath, t)

    return True


def EncryptFile(ofilepath, nfilepath):
    if not Path(ofilepath).exists():
        return False

    fb = isCrypted(ofilepath)
    if fb is not None:
        copy(ofilepath, nfilepath)
        return True

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

    # 保存新文件
    save(nfilepath, t)

    return True