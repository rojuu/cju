//Line comment at the top;

int add(int a, int b) //line comment here
{
    /*

        block comment with stuff

        slkdjf
    */
    return a + b;
}
// line comment

int foo(int a, int b)
{
    return a && b;
}

int bar(int a, int b)
{
    return (a << b) || (a <= b);
}

int main(void)
{
    return add(1, sub_numbers(3 & 1, 4));
}