//DISCLAIMER: I DO NOT SUPPORT PEOPLE PLAGIARIZING MY CODE. I DO NOT TAKE RESPONSIBILITY FOR THE UNLAWFUL ACTIONS OF OTHERS.

#include <cstdlib>
#include <string>
#include <iostream>
#include <fstream> //Files
#include <sstream> //File parsing
#include <memory>  //Smart pointers

// FIFO
#include <queue>
#include <unordered_set>

// LRU
#include <list>
#include <unordered_map>

using namespace std;

struct Page
{
    int page_number;
    bool is_modified;

    Page(int num, bool modified) : page_number(num), is_modified(modified) {}

    bool operator==(const Page &other) const
    {
        return page_number == other.page_number; // Assuming page number uniquely identifies a page
    }
};

struct PageHash
{
    size_t operator()(const Page &page) const
    {
        return hash<int>()(page.page_number); // Simple hash using the page number
    }
};

class PageReplacementAlgorithm
{
protected:
    ofstream &out_file; // Reference to the output file for writing
    int total_references = 0, read_faults = 0, write_faults = 0;
    int total_swap_in_time = 0, total_swap_out_time = 0;
    int swap_in_cost, swap_out_cost;

public:
    PageReplacementAlgorithm(ofstream &out, int in_cost, int out_cost) : out_file(out), swap_in_cost(in_cost), swap_out_cost(out_cost) {}

    virtual ~PageReplacementAlgorithm() {}

    virtual void access_page(int page_number, bool is_modified) = 0;

    void print_summary() const
    {
        cout << "Total number of page references: " << total_references << endl;
        cout << "Total number of page faults on read references: " << read_faults << endl;
        cout << "Total number of page faults on write references: " << write_faults << endl;
        cout << "Total number of time units for swapping in pages: " << total_swap_in_time << endl;
        cout << "Total number of time units for swapping out pages: " << total_swap_out_time << endl;
    }
};

class FIFOPageReplacement : public PageReplacementAlgorithm
{
private:
    queue<int> page_queue;              // Queue to keep track of pages in memory
    unordered_set<int> pages_in_memory; // Set to quickly check if a page is in memory
    size_t capacity;                    // Maximum number of pages that can be in memory

public:
    FIFOPageReplacement(ofstream &out, int cap, int in_cost, int out_cost) : PageReplacementAlgorithm(out, in_cost, out_cost), capacity(cap) {}

    void access_page(int page_number, bool is_modified) override
    {
        bool page_fault = pages_in_memory.find(page_number) == pages_in_memory.end();
        int victim_page_number = -1;
        int swap_in_time = 0, swap_out_time = 0;

        if (page_fault)
        {
            if (page_queue.size() >= capacity)
            {
                victim_page_number = page_queue.front();
                page_queue.pop();
                pages_in_memory.erase(victim_page_number);
                swap_out_time = swap_out_cost;
            }
            page_queue.push(page_number);
            pages_in_memory.insert(page_number);
            swap_in_time = swap_in_cost;
        }

        // Write operation to output file
        out_file << ++total_references << ": " << (is_modified ? "W" : "R") << " "
                 << page_number << " " << (page_fault ? "F" : "H") << " "
                 << victim_page_number << " " << swap_in_time << " " << swap_out_time << endl;

        if (page_fault)
        {
            total_swap_in_time += swap_in_cost;
            if (is_modified)
                write_faults++;
            else
                read_faults++;
        }
        if (victim_page_number != -1)
            total_swap_out_time += swap_out_cost;
    }
};

class LRUPageReplacement : public PageReplacementAlgorithm
{
private:
    list<int> pages;                                  // List to keep track of page order (front is most recent)
    unordered_map<int, list<int>::iterator> page_map; // Map to find pages quickly
    size_t capacity;                                  // Maximum number of pages that can be in memory

public:
    LRUPageReplacement(ofstream &out, int cap, int in_cost, int out_cost) : PageReplacementAlgorithm(out, in_cost, out_cost), capacity(cap) {}

    void access_page(int page_number, bool is_modified) override
    {
        auto it = page_map.find(page_number);
        bool page_fault = it == page_map.end();
        int victim_page_number = -1;
        int swap_in_time = 0, swap_out_time = 0;

        if (page_fault)
        {
            if (pages.size() >= capacity)
            {
                victim_page_number = pages.back();
                page_map.erase(victim_page_number);
                pages.pop_back();
                swap_out_time = swap_out_cost;
            }
            pages.push_front(page_number);
            page_map[page_number] = pages.begin();
            swap_in_time = swap_in_cost;
        }
        else
        {
            pages.splice(pages.begin(), pages, it->second); // Move to front
            page_map[page_number] = pages.begin();
        }

        // Write operation to output file
        out_file << ++total_references << ": " << (is_modified ? "W" : "R") << " "
                 << page_number << " " << (page_fault ? "F" : "H") << " "
                 << victim_page_number << " " << swap_in_time << " " << swap_out_time << endl;

        if (page_fault)
        {
            total_swap_in_time += swap_in_cost;
            if (is_modified)
                write_faults++;
            else
                read_faults++;
        }
        if (victim_page_number != -1)
            total_swap_out_time += swap_out_cost;
    }
};

int main(int argc, char *argv[])
{
    if (argc != 7)
    {
        cerr << "usage: " << argv[0] << " <algorithm> <num_frames> <swap_in_cost> <swap_out_cost> <input_file> <output_file>\n";
        exit(EXIT_FAILURE);
    }

    // parsing the arguments
    string algorithm = argv[1];
    int num_frames = stoi(argv[2]);
    int swap_in_cost = stoi(argv[3]);
    int swap_out_cost = stoi(argv[4]);
    string input_file = argv[5];
    string output_file = argv[6];

    if (algorithm != "LRU" && algorithm != "FIFO")
    {
        cerr << "Invalid algorithm specified. Use 'LRU' or 'FIFO'.\n";
        exit(EXIT_FAILURE);
    }
    // output the parsed values
    cout << "algorithm: " << algorithm << endl;
    cout << "number of frames: " << num_frames << endl;
    cout << "swap-in cost: " << swap_in_cost << endl;
    cout << "swap-out cost: " << swap_out_cost << endl;
    cout << "input file: " << input_file << endl;
    cout << "output file: " << output_file << endl;

    // making sure the input and output files can be opened
    ifstream in_file(input_file);
    if (!in_file.is_open())
    {
        cerr << "error: unable to open input file '" << input_file << "'\n";
        exit(EXIT_FAILURE);
    }
    ofstream out_file(output_file);
    if (!out_file.is_open())
    {
        cerr << "error: unable to open output file '" << output_file << "'\n";
        exit(EXIT_FAILURE);
    }

    unique_ptr<PageReplacementAlgorithm> pra;
    if (algorithm == "FIFO")
    {
        pra = make_unique<FIFOPageReplacement>(out_file, num_frames, swap_in_cost, swap_out_cost);
    }
    else if (algorithm == "LRU")
    {
        pra = make_unique<LRUPageReplacement>(out_file, num_frames, swap_in_cost, swap_out_cost);
    }
    else
    {
        cerr << "Invalid algorithm specified. Use 'LRU' or 'FIFO'.\n";
        exit(EXIT_FAILURE);
    }

    string line;
    int line_count = 0;
    while (getline(in_file, line))
    {
        istringstream string_stream(line);
        char op; // Operation type 'R' or 'W'
        int page_number;

        if (!(string_stream >> op >> page_number))
        {
            cerr << "Error parsing line: " << line << endl;
            continue;
        }

        bool is_modified = (op == 'W'); // Write operation indicates modification
        pra->access_page(page_number, is_modified);
    }

    // After processing all page references, print summary statistics
    pra->print_summary();

    // Cleanup
    in_file.close();
    out_file.close();

    return 0;
}
