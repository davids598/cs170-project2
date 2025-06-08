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
#include <chrono>
using namespace std;

pair<string, int> Input();
void Normalize(vector<vector<float> >&, int);
void Search(vector<vector<float> >, int, int);
float Accuracy(vector<vector<float> >&, vector<int>);
float Distance(vector<vector<float> >&, vector<int>&, int, int);

#define K_NN_COUNT 1

int main() {
    pair<string, int> return_vals = Input();
    //Start Timer
    auto start = chrono::high_resolution_clock::now();
    auto start_data_prep = chrono::high_resolution_clock::now();
    //Read in values from selected data
    vector<vector<float> > data;
    //Calculates number of features in this data set
    int data_size = 0;
    float number_calc;
    ifstream inputFile_calc;
    inputFile_calc.open(return_vals.first);
    string line;
    getline(inputFile_calc, line);
    stringstream ss(line);
    float temp_number;
    while (ss >> temp_number) {
        data_size++;
        if (ss.peek() == '/n') {
            break;
        }
    }
    inputFile_calc.close();
    //Actually reads in the data and stores it in the data 2D vector
    float number;
    ifstream inputFile;

    inputFile.open(return_vals.first);
    vector<float> temp;
    while (!inputFile.eof()) {
        inputFile >> number;
        if (temp.size() == data_size) {
            data.push_back(temp);
            temp.erase(temp.begin(), temp.end());
        }
        //cout << number << endl;
        temp.push_back(number);
    }
    inputFile.close();
    cout << "Dataset has " << data.at(0).size()-1 << " features (not including class label), with " << data.size() << " instances." << endl;
    /*
    bool error = false;
    //Print data for testing
    for (int i = 0; i < data.size(); ++i) {
        if (data.at(i).size() != data_size) {
            cout << "ERROR IN SIZE VALUE!!!!" << endl;
            error = true;
        } else {
            cout << "WE ARE FINE" << endl;
        }
        for (int j = 0; j < data.at(i).size(); ++j) {
            cout << data.at(i).at(j) << " ";
        }
        cout << endl;
    }
    cout << data_size << endl;
    cout << error << endl;
    cout << data.size() << " " << data.at(0).size() << endl;
    */
    Normalize(data, data_size);
    auto end_data_prep = chrono::high_resolution_clock::now();
    auto diff_data_prep = chrono::duration_cast<chrono::milliseconds>(end_data_prep - start_data_prep);
    cout << "Time used for data prep is: " << setprecision(4) << diff_data_prep.count() << " milliseconds" << endl;

    Search(data, return_vals.second, data_size-1);
    //End Timer and print to screen
    auto end = chrono::high_resolution_clock::now();

    //Different Prints depending on num features
    if (data_size > 20) {
        auto diff = chrono::duration_cast<chrono::seconds>(end - start);
        cout << "Time used for search is: " << setprecision(4) << diff.count() << " seconds" << endl;
    } else {
        auto diff = chrono::duration_cast<chrono::milliseconds>(end - start);
        cout << "Time used for search is: " << setprecision(4) << diff.count() << " milliseconds" << endl;
    }
    return 0;
}

pair<string, int> Input() {
    cout << "Welcome to David Strathman's Feature Selection Algorithm" << endl;
    cout << "Type in the name of the file to test: ";
    string input;
    cin >> input;
    cout << endl << endl;

    cout << "Type the number of the algorithm you would like to use" << endl;
    cout << "1. Forward Selection" << endl;
    cout << "2. Backward Elmination" << endl;
    int x;
    cin >> x;

    pair<string, int> inputs;
    inputs.first = input;
    inputs.second = x;
    return inputs;
}

void Normalize(vector<vector<float> > &data, int data_size) {
    int data_length = data_size - 1;
    int num_rows = data.size();

    float mean = -1;
    float std_dev = -1;

    float std_dev_sum = 0;
    float sum = 0;
    float z_score = -1;

    for (int i = 0; i < data_length; ++i) {
        //Debugging print
        /*
        cout << "Current data values for column " << to_string(i+1) << ": "<< endl;
        for (int j = 0; j < num_rows; ++j) {
            //Debug print
            cout << data.at(j).at(i+1) << endl;
        }
        */
        for (int j = 0; j < num_rows; ++j) {
            //Get sum to use for mean calculation
            sum += data.at(j).at(i+1);
        }
        //Calculate mean
        mean = sum / (float)num_rows;
        for (int j = 0; j < num_rows; ++j) {
            //Calculate values for std dev
            float cur_val = data.at(j).at(i+1);
            std_dev_sum += pow((cur_val - mean), 2);
        }
        std_dev = sqrt((std_dev_sum / (float)num_rows));
        //Now time to actually update values
        for (int j = 0; j < num_rows; ++j) {
            //Update values
            float cur_val = data.at(j).at(i+1);
            z_score = ((cur_val - mean) / std_dev);
            data.at(j).at(i+1) = z_score;
        }
        //Clear data for next run
        mean = -1;
        std_dev = -1;
        std_dev_sum = 0;
        sum = 0;
        z_score = -1;
        //Debugging print
        /*
        cout << "Final data values for column " << to_string(i+1) << ": "<< endl;
        for (int j = 0; j < num_rows; ++j) {
            //Debug print
            cout << data.at(j).at(i+1) << endl;
        }
        */
    }
    return;
}

void Search(vector<vector<float> > data, int choice, int data_length) {
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

    if (choice == 1) {
        //Forward Selection
        int count = 0;
        float default_rate; //Calculate Default rate for forward search
        for (int i = 0; i < data.size(); ++i) {
            if (data.at(i).at(0) == 1) {
                count++;
            }
        }
        if ((float)count / data.size() > .5) {
            default_rate = (float)count / data.size();
        } else {
            default_rate = (data.size()-count) / (float)data.size();
        }
        cout << "No Accuracy value with no features to test, the default rate is: " << default_rate*100 << "%" << endl << endl;
    } else {
        //Backward Elmination
        float acc = Accuracy(data, curr_features) * 100;
        cout << "Running Nearest Neighbor with all " << data.at(0).size()-1 << " features with 'leave one out' validation, I get an accuracy of " << acc << "%." << endl << endl;
    }

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
                float acc = Accuracy(data, frontier_search);
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
                float acc = Accuracy(data, frontier_search);
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

float Accuracy(vector<vector<float> > &data, vector<int> feature_set) {
    int correct = 0;
    vector<pair<float, int> > distances;
    for (int i = 0; i < data.size(); ++i) { //For each instance, find closest neighbor
        int label = data.at(i).at(0);
        int loc = -1;
        float nearest_neighbor_dist = numeric_limits<float>::max();
        int calc_label = -1;

        for (int k = 0; k < data.size(); ++k) {
            if (k != i) { //Don't test against yourself
                float distance = Distance(data, feature_set, i, k); //Calculate distance between instance i and current neighbor k
                if (distance < nearest_neighbor_dist) {
                    nearest_neighbor_dist = distance;
                    loc = k;
                    calc_label = data.at(k).at(0);
                }
                if (K_NN_COUNT > 1 && K_NN_COUNT % 2 == 1) { //Gather all distances if we are in KNN > 1 mode
                    distances.push_back(make_pair(distance, data.at(k).at(0)));
                }
            }
        }

        if (calc_label == label && K_NN_COUNT == 1) { //If correctly predicted, update count of correct predictions
            correct++;
            //cout << "Correct Updated!" << endl;
        } else if (K_NN_COUNT > 1 && K_NN_COUNT % 2 == 1) { // If in KNN > 1 mode, get all distances and labels
            int temp_count = 0;
            sort(distances.begin(), distances.end()); //Sort from closest to farthest nodes
            for (int k = 0; k < K_NN_COUNT; ++k) {
                int calc_label = distances.at(k).second;
                if (calc_label == label) { //Count correct number of neighbors
                    temp_count++;
                }
            }
            if ((float)temp_count > ((float)K_NN_COUNT / (float)2)) { //If correct # of neighbors are a majority, mark as correct
                correct++;
            }
        }
        distances.clear();
    }
    float accuracy = (float)correct / (float)data.size(); //Return accuracy
    //cout << "Data size is " << data.size() << endl;
    //cout << "Correct is " << correct << endl;
    //cout << "Accuracy is " << accuracy << endl;
    return accuracy;
}

float Distance(vector<vector<float> > &data, vector<int> &feature_set, int i, int k) {
    float sum = 0; //sum Euclidean distance for every selected feature and then get final distance calc
    //cout << "i is: " << i << " and k is: " << k << endl;
    for (int j = 0; j < feature_set.size(); ++j) {
        float diff = abs(data.at(i).at(feature_set.at(j)) - data.at(k).at(feature_set.at(j)));
        //cout << "Diff is: " << diff << " ";
        sum += diff * diff;
    }
    //cout << endl;
    return sqrt(sum);
}