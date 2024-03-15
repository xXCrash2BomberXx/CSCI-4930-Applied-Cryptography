from Crypto.Util.number import getPrime

def Diffie_Hellman(p: int, pubKey: int, privKey: int) -> int:
    return pow(pubKey, privKey, p)

def genPublicKey(g: int, p: int, privKey: int):
    return pow(g, privKey, p)

if __name__ == "__main__":
    p = 23
    g = 5
    a = 6
    A = genPublicKey(g, p, a)
    b = 15
    B = genPublicKey(g, p, b)
    print(Diffie_Hellman(p, A, b))
    print(Diffie_Hellman(p, A, b) == Diffie_Hellman(p, B, a))
    
    p = getPrime(1024)
    g = 5
    a = getPrime(1024)
    A = genPublicKey(g, p, a)
    b = getPrime(1024)
    B = genPublicKey(g, p, b)
    print(Diffie_Hellman(p, A, b))
    print(Diffie_Hellman(p, A, b) == Diffie_Hellman(p, B, a))
    