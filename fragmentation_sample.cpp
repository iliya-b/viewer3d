
#include <vector>


struct TestInt {
    int i_;
    TestInt(int i): i_(i) {
    }
    ~TestInt()
    { 
        // std::cout << "int destroyed" << std::endl; 
    }

};

std::vector<TestInt> test(){

    std::vector<TestInt> v;
    for(int i =0;i<50000;i++){
        v.push_back(  TestInt(i));
    }
    return v;
}

int main()
{   

    while(true){
        auto t = test();

    }

    return 0;
}