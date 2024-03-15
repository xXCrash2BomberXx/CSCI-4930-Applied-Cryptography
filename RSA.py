from Crypto.Util.number import getPrime, inverse, getStrongPrime
from math import gcd


class RSA:
    '''
    def findCoprime(num: int) -> int:
        i = getPrime(1024)
        while gcd(num, i) != 1:
            i += 1
        return i

    def inverse(A, M):
        m0 = M
        y = 0
        x = 1
        if (M == 1):
            return 0
        while (A > 1):
            q = A // M
            t = M
            M = A % M
            A = t
            t = y
            y = x - q * y
            x = t
        if (x < 0):
            x = x + m0
        return x
    '''
    def keyGen():
        p: int = getPrime(1024)
        q: int = getPrime(1024)
        N: int = p*q
        phi: int = (p-1)*(q-1)
        e = getStrongPrime(1024, e=phi)  # RSA.findCoprime(phi)
        d = inverse(e, phi)
        return (e, d, N)

    def encrypt(message: int, publicKey: int, N: int) -> int:
        return pow(message, publicKey, N)

    def encryptString(message: str, publicKey: int, N: int) -> int:
        num = ""
        for char in message:
            s = bin(ord(char))[2:]
            while len(s) < 8:
                s = "0"+s
            num += s
        return RSA.encrypt(int(num, 2), publicKey, N)

    def decrypt(message: int, privateKey: int, N: int) -> int:
        return RSA.encrypt(message, privateKey, N)

    def decryptString(message: str, privateKey: int, N: int) -> str:
        d = bin(RSA.decrypt(message, privateKey, N))[2:]
        while len(d) % 8 != 0:
            d = "0"+d
        s = ""
        for i in range(0, len(d), 8):
            s += chr(int(d[i:i+8], 2))
        return s


e, d, N = RSA.keyGen()

message = "Hello, World!"
encrypted = RSA.encryptString(message, e, N)
decrypted = RSA.decryptString(encrypted, d, N)
