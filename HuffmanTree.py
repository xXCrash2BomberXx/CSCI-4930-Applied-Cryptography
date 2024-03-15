from string import printable
from random import shuffle, randint

class HuffmanTree:
    class Node:
        def __init__(self, left=None, right=None):
            self.left = left
            self.right = right
            
    def __init__(self, chars: list[str] = [c for c in printable]):
        shuffle(chars)
        self.root = HuffmanTree.Node()
        ptr = self.root
        while len(chars) > 2:
            if randint(0, 1):
                ptr.left = chars[0]
                ptr.right = HuffmanTree.Node()
                ptr = ptr.right
            else:
                ptr.left = HuffmanTree.Node()
                ptr.right = chars[0]
                ptr = ptr.left
            chars = chars[1:]
        ptr.left = chars[0]
        ptr.right = chars[1]
        
    def genKey(self):
        s = ""
        ptr = self.root
        while isinstance(ptr.left, HuffmanTree.Node) or isinstance(ptr.right, HuffmanTree.Node):
            t = randint(0, 1)
            t2 = int(not t)
            s += str(t)+str(t2)
            if isinstance(ptr.left, HuffmanTree.Node):
                ptr = ptr.left
            else:
                ptr = ptr.right
        t = randint(0, 1)
        t2 = int(not t)
        s += str(t)+str(t2)
        return s
    
    def encode(self, plaintext, key):
        s = ""
        for c in plaintext:
            t = ""
            ptr = self.root
            i = 0
            while True:
                if ptr.left == c:
                    t += key[i]
                    break
                elif ptr.right == c:
                    t += key[i+1]
                    break
                elif isinstance(ptr.left, HuffmanTree.Node):
                    ptr = ptr.left
                    t += key[i]
                    i += 2
                else:
                    ptr = ptr.right
                    t += key[i+1]
                    i += 2
            s += t
        return s
        
    
    def decode(self, ciphertext, key):
        s = ""
        ptr = self.root
        i = 0
        for c in ciphertext:
            if c == key[i]:
                if isinstance(ptr.left, HuffmanTree.Node):
                    ptr = ptr.left
                    i += 2
                else:
                    s += ptr.left
                    ptr = self.root
                    i = 0
            else:  # c == key[i+1]
                if isinstance(ptr.right, HuffmanTree.Node):
                    ptr = ptr.right
                    i += 2
                else:
                    s += ptr.right
                    ptr = self.root
                    i = 0
        return s
    
x = HuffmanTree()
y = x.genKey()
z = x.encode("Hello, World!", y)
print(z)
print(x.decode(z, y))
