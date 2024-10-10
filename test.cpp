#include <iostream>
#include <chrono>
#include <unordered_map>
#include <vector>



typedef long long ll;

// Returns the number in the lexicographical order of all combinations of n numbers
// of the provided combination. 
ll code(std::vector<int> a,int n)
{
    std::sort(a.begin(),a.end());
    int cur = 0;
    int m = a.size();

    ll res =0;
    for(int i=0;i<a.size();i++)
    {
        if(a[i] == cur+1)
        {
            res++;
            cur = a[i];
            continue;
        }
        else
        {
            res++;
            int number_of_greater_nums = n - a[i];
            for(int j = a[i]-1,increment=1;j>cur;j--,increment++)
                res += 1LL << (number_of_greater_nums+increment);
            cur = a[i];
        }
    }
    return res;
}
// Takes the lexicographical code of a combination of n numbers and returns the 
// combination
std::vector<int> decode(ll kod, int n)
{
    std::vector<int> res;
    int cur = 0;

    int left = n; // Out of how many numbers are we left to choose.
    while(kod)
    {
        ll all = 1LL << left;// how many are the total combinations
        for(int i=n;i>=0;i--)
        {
            if(all - (1LL << (n-i+1)) +1 <= kod)
            {
                res.push_back(i);
                left = n-i;
                kod -= all - (1LL << (n-i+1)) +1;
                break;
            }
        }
    }
    return res;
}

int main () {
    std::vector<int> v = {1,4};
    auto start = std::chrono::high_resolution_clock::now();

    
    ll num = code(v, 3);
    
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> elapsed = end - start;

    std::cout<<num<<std::endl;
    std::cout << "Elapsed time: " << elapsed.count() << " ms" << std::endl;

    return 0;
};

