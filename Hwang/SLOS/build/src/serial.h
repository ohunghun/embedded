
void SerialInit(void);


int  SerialInputString(char *s, const int len, const int timeout);


void PutChar(const char ch);


void PutString(const char *ch) ;//문잘열을 출력하기 위해 구현해 놓았지만 printf문이 있어 거의 사용하지 않는다.



char GetChar(); //시리얼로 입력된 데이타를 가져 온다.


void GetString(char* str);

