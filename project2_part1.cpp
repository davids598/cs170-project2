#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>
#include <string>
#include <utility>
#include <algorithm>
#include <limits>
#include <cmath>
#include <sstream>
#include <cstdlib>
#include <ctime>
using namespace std;

pair<int, int> Input();
void Search(int, int);
float Accuracy(vector<int>);

int main() {
    pair<int, int> return_vals = Input();
    Search(return_vals.second, return_vals.first);
    return 0;
}

pair<int, int> Input() {
    cout << "Welcome to David Strathman's Feature Selection Algorithm" << endl;
    cout << "Please enter the total number of features: ";
    int input;
    cin >> input;
    cout << endl << endl;

    cout << "Type the number of the algorithm you would like to use" << endl;
    cout << "1. Forward Selection" << endl;
    cout << "2. Backward Elmination" << endl;
    int x;
    cin >> x;

    pair<int, int> inputs;
    inputs.first = input;
    inputs.second = x;
    return inputs;
}

void Search(int choice, int data_length) {
    vector<int> curr_features; //Have 2 vectors for curr state and all possible modifications
    vector<int> avail_features;
    if (choice == 1) {
        //Forward Selection
        for (int i = 0; i < data_length; ++i) {
            avail_features.push_back(i+1);
        }
    } else {
        //Backward Elmination
        for (int i = 0; i < data_length; ++i) {
            curr_features.push_back(i+1);
            avail_features.push_back(i+1);
        }
    }

    srand(time(0));
    float default_rate = (float)rand() / (float)RAND_MAX; //Calculate Default rate randomly for now
    cout << "No Accuracy value with no features to test, the default rate is: " << default_rate*100 << "%" << endl << endl;

    cout << "Beginning Search" << endl << endl;

    float best_acc = 0; //Store best accuracy and features to get that accuracy
    vector<int> best_features;

    while (avail_features.size() != 0) {
        vector<float> accs;
        stringstream output_data;
        if (choice == 1) {
            //Forward Selection, generate all possible next nodes and calcuate accuracy values
            for (int i = 0; i < avail_features.size(); ++i) {
                vector<int> frontier_search = curr_features;
                frontier_search.push_back(avail_features.at(i));
                float acc = Accuracy(frontier_search);
                accs.push_back(acc);
                output_data << "Using features(s) {";
                for (int i = 0; i < frontier_search.size(); ++i) {
                    if (i == frontier_search.size() - 1) {
                        output_data << frontier_search.at(i);
                    } else {
                        output_data << frontier_search.at(i) << ",";
                    }
                }
                output_data << "} accuracy is " << acc*100 << "%" << endl;
            }
        } else {
            //Backwards Elmination, generate all possible next nodes and calcuate accuracy values
            for (int i = 0; i < avail_features.size(); ++i) {
                vector<int> frontier_search = curr_features;
                frontier_search.erase(frontier_search.begin() + i);
                float acc = Accuracy(frontier_search);
                accs.push_back(acc);
                output_data << "Using features(s) {";
                for (int i = 0; i < frontier_search.size(); ++i) {
                    if (i == frontier_search.size() - 1) {
                        output_data << frontier_search.at(i);
                    } else {
                        output_data << frontier_search.at(i) << ",";
                    }
                }
                output_data << "} accuracy is " << acc*100 << "%" << endl;
            }
        }
        //Find Best feature, add to curr set, update best acc and avail_features, and print.
        //Also print if accuracy has decreased since the last step
        //Empty Accs(list of all accuracy values for each frontier node)
        cout << output_data.str(); //Flush output stream to print to terminal
        output_data.flush();
        auto max = max_element(accs.begin(), accs.end()); //Get best accuracy and position
        int index = distance(accs.begin(), max);
        if (*max < best_acc) { //Print if no improvement compared to best accuracy
            cout << endl;
            cout << "Accuracy has decreased!! Still continuing";
        }

        if (choice == 1) { //Forward Search print for best feature set this iteration
            cout << endl;
            cout << "Best feature set was {";
            for (int i = 0; i < curr_features.size(); ++i) {
                cout << curr_features.at(i) << ",";
            }
            cout << avail_features.at(index) << "}, accuracy is " << *max*100 << "%" << endl;
            cout << endl;
        } else { //Backward Elimination print for best feature set this iteration
            cout << endl;
            cout << "Best feature set was {";
            for (int i = 0; i < curr_features.size(); ++i) {
                if (i != index) {
                    if (i == curr_features.size() - 1) {
                        cout << curr_features.at(i);
                    } else {
                        cout << curr_features.at(i) << ",";
                    }
                }
            }
            cout << "}, accuracy is " << *max*100 << "%" << endl;
            cout << endl;
        }
        
        if (*max > best_acc) { //Update best accuracy if we have a new best accuracy
            best_acc = *max;
            if (choice == 1) { //Need to update best_features array if new best feature set found
                best_features = curr_features;
                best_features.push_back(avail_features.at(index));
            } else { //Same update of best_features, but for backward search
                best_features = curr_features;
                best_features.erase(best_features.begin()+index);
            }
        }
        //Update vectors to prepare for next iteration of search
        //Update curr set and avail features
        if (choice == 1) {
            //Add to curr and remove from avail in Forward
            curr_features.push_back(avail_features.at(index));
            avail_features.erase(avail_features.begin()+index);
        } else {
            //Remove from curr and avail for backward
            curr_features.erase(curr_features.begin()+index);
            avail_features.erase(avail_features.begin()+index);
        }
        //Wipe Accs
        accs.erase(accs.begin(), accs.end());

    }
    //Final print at end of while loop, print best accuracy list and percentage correct
    cout << endl;
    cout << "Finished Search!!! The best feature subset is {";
    for (int i = 0; i < best_features.size(); ++i) {
        if (i == best_features.size() - 1) {
            cout << best_features.at(i);
        } else {
            cout << best_features.at(i) << ",";
        }
    }
    cout << "}, accuracy is " << best_acc*100 << "%" << endl;
}

float Accuracy(vector<int> feature_set) {
    float accuracy = (float)rand() / (float)RAND_MAX; 
    return accuracy;
}