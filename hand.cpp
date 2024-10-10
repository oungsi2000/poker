#include <iostream>
#include <vector>
#include <random>
#include <map>
#include <algorithm>
#include <tuple>
#include <chrono>
#include <cmath>


using Pair = std::pair<int, char>;
using HandResult = std::tuple<int, int, int>; //tuple (ranking, rank_num, kicker)

namespace Hands {

    //J = 11, Q=12, K=13, A=14
    int numbers[13] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 0xB, 0xC, 0xD, 0xE};
    char patterns[4] = {'s', 'd', 'h', 'c'};

    std::vector<Pair> deck;
    std::random_device rd;
    std::mt19937 gen(rd());
};

enum HandRankings {
    high_card,
    one_pair,
    two_pair,
    three_of_a_kind,
    straight,
    flush,
    full_house,
    four_of_a_kind,
    straight_flush,
};

std::vector<Pair> init_deck() {
    using namespace Hands;

    for (int i = 0; i < sizeof(numbers)/sizeof(int); ++i) {
        for (int j = 0; j < sizeof(patterns)/sizeof(char); ++j) {
            deck.push_back({numbers[i], patterns[j]});
        }
    }
    return deck;
};

Pair peek_one() {
    std::uniform_int_distribution<> distr(0, Hands::deck.size() - 1);
    int target_index = distr(Hands::gen);

    Pair random_value = Hands::deck[target_index];
    Hands::deck.erase(Hands::deck.begin() + target_index);

    return random_value;
}

unsigned int ranks(Pair whole_board[7]) {
    
    bool is_one_pair = false;
    bool is_two_pair = false;
    bool is_three_of_a_kind = false;
    bool is_straight = false;
    bool is_flush = false;
    bool is_full_house = false;
    bool is_four_of_a_kind = false;
    bool is_straight_flush = false;

    int pair_count = 0;
    int continuous_count = 0;
    int flush_count = 0;
    int rank_num = 0;
    int straight_flush_count = 0;
    int full_house_rank_num = 0;
    int prev_pair = 0;

    int kicker = 0;


    char target_pattern = 'n';

    std::map<int, int> num_count; //map : (num, duplication)
    std::vector<Pair> sorted_board;
    
    //parse whole board
    for (int i = 0; i < 7; i++) {
        num_count[whole_board[i].first]++;
    }

    //sort whole_board
    for (int i = 0; i < 7; i++) {
        sorted_board.push_back(whole_board[i]);
    }

    sort(sorted_board.begin(), sorted_board.end());
    //pair count : for one pair, two pair, three of a kind, four of a kind

    int prev_num = 200;
    char prev_pattern = 'n';
    int idx = 0;
    std::map<char, int> pattern_count;

    //hand eval -> 0x00000000
    for (auto& num : num_count) {

        if(num.second == 2){
            is_one_pair = true;
            rank_num = num.first;
            pair_count +=1;
        }

        if(num.second == 3){
            is_three_of_a_kind = true;
            rank_num = num.first;
            full_house_rank_num = num.first;
        }

        if (num.second == 4) {
            is_four_of_a_kind = true;
            rank_num = num.first;
        }

        if (pair_count >= 2 && num.second == 2) {
            is_two_pair = true;
            rank_num = num.first*16+prev_pair;
        }

        if(num.second == 2){
            prev_pair = num.first;
        }

        if (is_three_of_a_kind && is_one_pair) {
            is_full_house = true;
            rank_num = full_house_rank_num*16+prev_pair;
        }
    }

    for (auto num : sorted_board) {

        // for flush
        pattern_count[num.second]++;
        for (auto pattern : pattern_count){
            if(pattern.second >= 5 && !(straight_flush_count > 3)) {
                is_flush = true;
                rank_num = num.first;

                int count = 0;
                for (int i = 2; i< 8; i++) { 
                    if (sorted_board[idx-i].second == pattern.first) {
                        kicker = kicker + sorted_board[idx-i].first*(4096 / pow(16, count));
                        count++;
                    }
                }
            }
        }
        
        // for straight and straight flush
        
        if (num.first - 1 == prev_num) {
            continuous_count +=1;
            
            if (continuous_count > 3) {
                is_straight = true;
                rank_num = num.first;
            } 
            if (num.second == prev_pattern || num.second == target_pattern) {
                if (!(target_pattern != 'n' && num.second != target_pattern)) {
                    straight_flush_count += 1;
                } 

                if (straight_flush_count > 3) {
                    is_straight_flush = true;
                    rank_num = num.first;
                } else if (flush_count > 1) {
                    target_pattern = num.second;
                }
            } 

        } else {
            continuous_count = 0;
            straight_flush_count = 0;
        }

        prev_num = num.first;
        prev_pattern = num.second;
        idx++;
    }
    
    //kicker calculation
    int count = 0;
    auto t = num_count.rbegin();

    //for back straight A 2 3 4 5
    if (sorted_board.rbegin()->first == 14){
        
        char new_valc = sorted_board.rbegin()->second;
        for (auto& p : sorted_board) {
            if (p.first == 14) {
                p.first = 1;
            }
        }

        continuous_count = 0;
        straight_flush_count = 0;
        prev_pattern = 'n'; 
        target_pattern = 'n';

        for (auto& num : sorted_board) {
            if (num.first - 1 == prev_num) {
                continuous_count +=1;
                
                if (continuous_count > 3) {
                    is_straight = true;
                    rank_num = num.first;
                } 
                if (num.second == prev_pattern || num.second== target_pattern) {
                    if (!(target_pattern != 'n' && num.second != target_pattern)) {
                        straight_flush_count += 1;
                    }

                    if (straight_flush_count > 3) {
                        is_straight_flush = true;
                        rank_num = num.first;
                    } else if (flush_count > 1) {
                        target_pattern = num.second;
                    }
                } 
            } else {
                continuous_count = 0;
                straight_flush_count = 0;

            }
            prev_num = num.first;
            prev_pattern = num.second;
        } 
    }

    //return with kicker, 

    if(is_straight_flush){
        kicker = 0000;
        return straight_flush*16777216+rank_num*1048576+kicker;

    } else if (is_four_of_a_kind) {
        while (count < 1) {
            if (rank_num != t->first) {
                kicker = kicker*16+t->first;
                ++count;
            }
            ++t;
        }
        kicker = kicker*4096;
        return four_of_a_kind*16777216+rank_num*1048576+kicker;

    } else if (is_full_house) {
        kicker = 0000;
        return full_house*16777216+rank_num*1048576+kicker;

    } else if (is_flush) {
        //defined in flush method
        return flush*16777216+rank_num*1048576+kicker;

    } else if (is_straight) {
        kicker = 0000;
        return straight*16777216+rank_num*1048576+kicker;

    } else if (is_three_of_a_kind) {
        while (count < 2) {
            if (rank_num != t->first) {
                kicker = kicker*16+t->first;
                ++count;
            }
            ++t;
        }
        kicker = kicker*256;
        return three_of_a_kind*16777216+rank_num*1048576+kicker;

    } else if (is_two_pair) {

        while (count < 1) {
            if (rank_num != t->first) {
                kicker = kicker*16+t->first;
                ++count;
            }
            ++t;
        }
        kicker = kicker*4096;
        return two_pair*16777216+rank_num*1048576+kicker;

    } else if (is_one_pair) {

        while (count < 3) {
            if (rank_num != t->first) {
                kicker = kicker*16+t->first;
                ++count;
            }
            ++t;
        }
        kicker = kicker*16;
        return one_pair*16777216+rank_num*1048576+kicker;

    } else {
        
        auto t = num_count.rbegin();
        rank_num = t->first;

        for (int i=0; i<4; i++) {
            ++t;
            kicker = kicker*16+t->first;   
        }
        return high_card*16777216+rank_num*1048576+kicker;
    }
}; 
// 0xrank1, rank_num2, kicker4

//0x30E0BA00
int main () {

    std::vector<Pair> set_deck = init_deck();
    auto start = std::chrono::high_resolution_clock::now();

    for (int i =0; i<100000; i++) {
        Hands::deck = set_deck;

        //index 0, 1 = player_hands, index 2~7 = community_boards
        Pair whole_board[7];
        for (int i = 0; i < 7; i++) {
            whole_board[i] = peek_one();
        }  

        whole_board[0] = {0xE, 's'};
        whole_board[1] = {0xE, 'd'};
        whole_board[2] = {0xE, 'd'};
        whole_board[3] = {7, 's'};
        whole_board[4] = {10, 's'};
        whole_board[5] = {5, 'c'};
        whole_board[6] = {0xB, 's'};

        unsigned int v = ranks(whole_board); 

      
        // std::cout<<std::hex<<v<<std::endl;
        
        // for (auto a : whole_board) { 
        //     std::cout<<a.first<<a.second<<' ';
        // }
    }
    auto end = std::chrono::high_resolution_clock::now();
  
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "Elapsed time: " << elapsed.count() << " ms" << std::endl;

    return 0;
};