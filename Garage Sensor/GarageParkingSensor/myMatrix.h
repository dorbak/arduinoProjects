byte upArrow[8] = {
  B00011000,
  B00111100,
  B01011010,
  B10011001,
  B00011000,
  B00011000,
  B00011000,
  B00011000
};
byte rightArrow[8] = {
  B00001000,
  B00000100,
  B00000010,
  B11111111,
  B11111111,
  B00000010,
  B00000100,
  B00001000,
};
byte downArrow[8] = {
  B00011000,
  B00011000,
  B00011000,
  B00011000,
  B10011001,
  B01011010,
  B00111100,
  B00011000
};
byte leftArrow[8] = {
  B00010000,
  B00100000,
  B01000000,
  B11111111,
  B11111111,
  B01000000,
  B00100000,
  B00010000,
};

byte circle[][8] = {
  {
    B11111111,
    B10000001,
    B10000001,
    B10000001,
    B10000001,
    B10000001,
    B10000001,
    B11111111
  },
  {
    B00000000,
    B01111110,
    B01000010,
    B01000010,
    B01000010,
    B01000010,
    B01111110,
    B00000000
  },
  {
    B00000000,
    B00000000,
    B00111100,
    B00100100,
    B00100100,
    B00111100,
    B00000000,
    B00000000
  },
  {
    B00000000,
    B00000000,
    B00000000,
    B00011000,
    B00011000,
    B00000000,
    B00000000,
    B00000000
  },
  {
    B10000001,
    B01000010,
    B00100100,
    B00011000,
    B00011000,
    B00100100,
    B01000010,
    B10000001
  }
};

byte digits[][8] = {   // 0
  {
    B0110,
    B1001,
    B1001,
    B1001,
    B1001,
    B1001,
    B1001,
    B0110
  },
  { // 1
    B0110,
    B1010,
    B0010,
    B0010,
    B0010,
    B0010,
    B0010,
    B0010
  },
  {
    B0110,             // 2
    B1001,
    B0001,
    B0010,
    B0100,
    B1000,
    B1000,
    B1111
  },
  {
    B1111,             // 3
    B0001,
    B0010,
    B0100,
    B1000,
    B0100,
    B0010,
    B1111
  },
  {
    B1001,             // 4
    B1001,
    B1001,
    B1111,
    B0001,
    B0001,
    B0001,
    B0001
  },
  {
    B1111,             // 5
    B1000,
    B1000,
    B1111,
    B0001,
    B0001,
    B0001,
    B1111
  },
  {
    B1111,             // 6
    B1000,
    B1000,
    B1000,
    B1110,
    B1001,
    B1001,
    B0110
  },
  {
    B1111,             // 7
    B0001,
    B0001,
    B0001,
    B0010,
    B0100,
    B1000,
    B1000
  },
  {
    B0110,             // 8
    B1001,
    B1001,
    B0110,
    B1001,
    B1001,
    B1001,
    B0110
  },
  {
    B0110,             // 9
    B1001,
    B1001,
    B0110,
    B0001,
    B0001,
    B0001,
    B1110
  }
};

byte A[8] = {
  B00000000,
  B00000000,
  B01111110,
  B00010001,
  B00010001,
  B01111110,
  B00000000,
  B00000000
};
byte B[8] = {
  B00000000,
  B00000000,
  B01111111,
  B01001001,
  B01001001,
  B00110110,
  B00000000,
  B00000000
};

byte stop[8] = {
  B00111100,
  B01000010,
  B10000001,
  B10000001,
  B10000001,
  B10000001,
  B01000010,
  B00111100,
};
byte smile01[8] = {
  B00111100,
  B01000010,
  B10010101,
  B10100001,
  B10100001,
  B10010101,
  B01000010,
  B00111100
};
byte smile02[8] = {
  B00111100,
  B01000010,
  B10010101,
  B10010001,
  B10010001,
  B10010101,
  B01000010,
  B00111100
};
byte smile03[8] = {
  B00111100,
  B01000010,
  B10100101,
  B10010001,
  B10010001,
  B10100101,
  B01000010,
  B00111100
};

void rotate (byte outArray[], byte inArray[])  // Rotate right
{
  int i, j, val;

  for (i = 0; i < 8; i++) {
    outArray[i] = 0;
  }
  
    //rotate 90* clockwise
    for (i = 0; i < 8; i++) {
      for (j = 0; j < 8; j++) {
        val = ((inArray[i] >> j) & 1); //extract the j-th bit of the i-th element
        outArray[7 - j] |= (val << i); //set the newJ-th bit of the newI-th element
      }
    
  }
}
// Function to copy 'len' elements from 'src' to 'dst'
void copy(int* src, int* dst, int len) {
    memcpy(dst, src, sizeof(src[0])*len);
}
void prntBits(byte b)
{
  for (int i = 7; i >= 0; i--)
  {
    Serial.print(bitRead(b, i));
  }
  Serial.println();
}



void printArray(byte *thisArray, int size = 8)
{
  for (int r = 0; r < size ; r++)
  {
    prntBits(thisArray[r]);
  }
}

void packChars(byte packedChars[], byte first[], byte second[])
{
  packedChars[8] = 0;
  for (int r = 0; r < 8; r++)
  {
    packedChars[r] = first[r];
    packedChars[r] = packedChars[r] * 16 ;
    packedChars[r] = packedChars[r] + second[r];

  }
}
