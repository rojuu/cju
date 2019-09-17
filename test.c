int add(int a, int b)
{
    return a + b;
}

int sub_numbers(int a, int b)
{
    return a - b;
}

int main(void)
{
    return add(1, sub_numbers(3 , 4));
}