#include <cmath>    // ceil
#include <fstream>  // ifstream
#include <iostream> // cout
#include <sstream>  // stringstream
#include <string>   // string
#include <vector>   // vector

class Hex
{
private:
  int *array;
  unsigned int _size{0};
  char *s = new char[2];
  unsigned int rounds{10};
  unsigned int RotWord(unsigned int b)
  {
    // https://cplusplus.com/forum/beginner/201001/
    return ((b << 8) | (b >> (sizeof(unsigned int) - 8))); // Left circular shift by 2 hex digits
  }
  unsigned int InverseRotWord(unsigned int b)
  {
    return ((b >> 8) | (b << (sizeof(unsigned int) - 8))); // Right circular shift by 2 hex digits
  }
  unsigned int SubWord(unsigned int b, std::string filename = "S-Box.txt")
  {
    std::ifstream file{filename};
    file.seekg((b / 16 + 1) % 16 * 52 + (b % 16 + 1) * 3);
    file.read(s, 2);
    file.close();
    b = std::stoul(s, nullptr, 16);
    return b;
  }
  unsigned int InverseSubWord(unsigned int b, std::string filename = "Inverse S-Box.txt")
  {
    return SubWord(b, filename);
  }

public:
  static int galoisMult(int a, int b)
  {
    // https://femionewin.medium.com/aes-encryption-with-python-step-by-step-3e3ab0b0fd6c
    int p{0};
    int hiBitSet{0};
    for (int i{0}; i < 8; i++)
    {
      if ((b & 1) == 1)
        p ^= a;
      hiBitSet = a & 0x80;
      a <<= 1;
      if (hiBitSet == 0x80)
        a ^= 0x1b;
      b >>= 1;
    }
    return p % 256;
  }
  struct Iterator
  {
  public:
    Iterator(int *ptr) : m_ptr(ptr) {}
    int &operator*() const { return *m_ptr; }
    int *operator->() { return m_ptr; }
    Iterator &operator++()
    {
      m_ptr++;
      return *this;
    }
    Iterator operator++(int)
    {
      Iterator tmp = *this;
      ++(*this);
      return tmp;
    }
    friend bool operator==(const Iterator &a, const Iterator &b)
    {
      return a.m_ptr == b.m_ptr;
    }
    friend bool operator!=(const Iterator &a, const Iterator &b)
    {
      return a.m_ptr != b.m_ptr;
    }

  protected:
    int *m_ptr;
  };
  Iterator begin() { return Iterator(&array[0]); }
  Iterator end() { return Iterator(&array[_size]); }
  Hex(std::string message, std::string key = "") : size(_size)
  {
    unsigned int temp = (int)ceil((float)message.size() / 16.);
    rounds = 10 + (key.size() / 8 - 2) * 2;
    _size = temp * 16;
    array = new int[_size];
    int index = message.size() - 1;
    for (int i = temp - 1; i >= 0; i--)
    {
      for (int j = 3; j >= 0; j--)
      {
        for (int k = 3; k >= 0; k--)
        {
          if (index >= 0)
            *(array + i * 16 + j * 4 + k) = (int)message[index--];
          else
            *(array + i * 16 + j * 4 + k) = 0;
        }
      }
    }
    encrypt(key);
  }
  Hex &encrypt(std::string key)
  {
    AddRoundKey(key, 0, false);
    for (unsigned int i{1}; i < rounds; i++)
    {
      // SubBytes();
      // ShiftRows();
      // MixColumns();
      AddRoundKey(key, i, false);
    }
    // SubBytes();
    // ShiftRows();
    AddRoundKey(key, rounds, false);
    return *this;
  }
  std::string decrypt(std::string key)
  {
    AddRoundKey(key, rounds, true);
    // InverseShiftRows();
    // InverseSubBytes();
    for (unsigned int i{rounds - 1}; i > 0; i--)
    {
      AddRoundKey(key, i, true);
      // InverseMixColumns();
      // InverseShiftRows();
      // InverseSubBytes();
    }
    AddRoundKey(key, 0, true);
    std::string message = "";
    for (int i = _size / 16 - 1; i >= 0; i--)
    {
      for (int j = 3; j >= 0; j--)
      {
        for (int k = 3; k >= 0; k--)
        {
          if (*(array + i * 16 + j * 4 + k) != 0x00)
            message = (char)*(array + i * 16 + j * 4 + k) + message;
        }
      }
    }
    return message;
  }
  Hex &SubBytes(std::string filename = "S-Box.txt")
  {
    for (int &i : *this)
    {
      std::ifstream file{filename};
      file.seekg((i / 16 + 1) * 52 + (i % 16 + 1) * 3);
      file.read(s, 2);
      file.close();
      i = std::stoul(s, nullptr, 16);
    }
    return *this;
  }
  Hex &InverseSubBytes(std::string filename = "Inverse S-Box.txt")
  {
    return SubBytes(filename);
  }
  Hex &ShiftRows()
  {
    for (int i{0}; i < _size; i += 16)
    {
      // Second Row Shift Left Once
      int a4 = array[i + 4];
      array[i + 4] = array[i + 5];
      array[i + 5] = array[i + 6];
      array[i + 6] = array[i + 7];
      array[i + 7] = a4;
      // Third Row Shift Left Twice
      int a8 = array[i + 8];
      int a9 = array[i + 9];
      array[i + 8] = array[i + 10];
      array[i + 9] = array[i + 11];
      array[i + 10] = a8;
      array[i + 11] = a9;
      // Fourth Row Shift Right Once
      int a15 = array[i + 15];
      array[i + 15] = array[i + 14];
      array[i + 14] = array[i + 13];
      array[i + 13] = array[i + 12];
      array[i + 12] = a15;
    }
    return *this;
  }
  Hex &InverseShiftRows()
  {
    for (int i{0}; i < _size; i += 16)
    {
      // Second Row Shift Right Once
      int a7 = array[i + 7];
      array[i + 7] = array[i + 6];
      array[i + 6] = array[i + 5];
      array[i + 5] = array[i + 4];
      array[i + 4] = a7;
      // Third Row Shift Left Twice
      int a8 = array[i + 8];
      int a9 = array[i + 9];
      array[i + 8] = array[i + 10];
      array[i + 9] = array[i + 11];
      array[i + 10] = a8;
      array[i + 11] = a9;
      // Fourth Row Shift Left Once
      int a12 = array[i + 12];
      array[i + 12] = array[i + 13];
      array[i + 13] = array[i + 14];
      array[i + 14] = array[i + 15];
      array[i + 15] = a12;
    }
    return *this;
  }
  Hex &MixColumns()
  {
    // https://femionewin.medium.com/aes-encryption-with-python-step-by-step-3e3ab0b0fd6c
    for (int i{0}; i < _size; i += 16)
      for (int j{0}; j < 4; j++)
      {
        int temp[] = {array[i + j], array[i + j + 4], array[i + j + 8], array[i + j + 12]};
        array[i + j] = galoisMult(temp[0], 2) ^ galoisMult(temp[3], 1) ^
                       galoisMult(temp[2], 1) ^ galoisMult(temp[1], 3);
        array[i + j + 4] = galoisMult(temp[1], 2) ^ galoisMult(temp[0], 1) ^
                           galoisMult(temp[3], 1) ^ galoisMult(temp[2], 3);
        array[i + j + 8] = galoisMult(temp[2], 2) ^ galoisMult(temp[1], 1) ^
                           galoisMult(temp[0], 1) ^ galoisMult(temp[3], 3);
        array[i + j + 12] = galoisMult(temp[3], 2) ^ galoisMult(temp[2], 1) ^
                            galoisMult(temp[1], 1) ^ galoisMult(temp[0], 3);
      }
    return *this;
  }
  Hex &InverseMixColumns()
  {
    for (int i{0}; i < _size; i += 16)
      for (int j{0}; j < 4; j++)
      {
        int temp[] = {array[i + j], array[i + j + 4], array[i + j + 8], array[i + j + 12]};
        array[i + j] = galoisMult(temp[0], 14) ^ galoisMult(temp[3], 9) ^
                       galoisMult(temp[2], 13) ^ galoisMult(temp[1], 11);
        array[i + j + 4] = galoisMult(temp[1], 14) ^ galoisMult(temp[0], 9) ^
                           galoisMult(temp[3], 13) ^ galoisMult(temp[2], 11);
        array[i + j + 8] = galoisMult(temp[2], 14) ^ galoisMult(temp[1], 9) ^
                           galoisMult(temp[0], 13) ^ galoisMult(temp[3], 11);
        array[i + j + 12] = galoisMult(temp[3], 14) ^ galoisMult(temp[2], 9) ^
                            galoisMult(temp[1], 13) ^ galoisMult(temp[0], 11);
      }
    return *this;
  }
  Hex &AddRoundKey(std::string key, unsigned int round = 0, bool inverse = false)
  {
    // https://en.wikipedia.org/wiki/AES_key_schedule
    int Nk{(int)rounds - 6}; // 4, 6, 8
    int w[4 * (Nk + 7)]{};
    int Rcon[]{0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1B, 0x36};
    for (int i{0}; i < Nk; i++)
    {
      w[i] = (key[4 * i] << 24) | (key[4 * i + 1] << 16) | (key[4 * i + 2] << 8) | key[4 * i + 3];
    }
    for (int i{Nk}; i < 4 * (Nk + 7); i++)
    {
      int temp{w[i - 1]};
      if (i % Nk == 0)
        temp = (inverse ? InverseSubWord(InverseRotWord(temp)) : SubWord(RotWord(temp))) ^ Rcon[i / Nk - 1];
      else if (Nk > 6 && i % Nk == 4)
        temp = (inverse ? InverseSubWord(temp) : SubWord(temp));
      w[i] = w[i - Nk] ^ temp;
    }
    for (unsigned int block{0}; block < _size; block += 16)
      for (int i{0}; i < 4; i++)
        for (int j{0}; j < 4; j++)
          array[block + j * 4 + i] ^= (w[round * 4 + i] >> (24 - 8 * j)) & 0xFF;
    return *this;
  }
  int &operator[](unsigned int i) { return array[i]; }
  int &at(int block, int row, int column)
  {
    return array[block * 16 + row * 4 + column];
  }
  std::string toString()
  {
    std::string temp = "";
    for (int i{0}; i < _size; i++)
    {
      std::stringstream stream;
      stream << std::hex << array[i];
      std::string sdig(stream.str());
      if (sdig.size() != 2)
        temp += "0" + sdig;
      else
        temp += sdig;
    }
    return temp;
  }
  ~Hex()
  {
    delete[] array;
    delete[] s;
  }
  const unsigned int &size;
};

int main(int argc, char *argv[])
{
  Hex h{(argc < 2) ? "___Hello, World!" : argv[1], (argc < 3) ? "Test Key" : argv[2]}; // 10, 12, 14
  /*
    _ _ _ H
    e l l o
    ,   W o
    r l d !
  */
  /*
  for (int i : h)
    std::cout << std::hex << i << std::endl;
  */
  std::cout << "0x" << h.toString() << std::endl;
  std::cout << h.decrypt((argc < 3) ? "Test Key" : argv[2]) << std::endl;
}
