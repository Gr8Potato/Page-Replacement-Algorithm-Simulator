# Page-Replacement-Algorithm-Simulator
This program simulates the behavior of different page replacement algorithms, specifically FIFO (First-In-First-Out) and LRU (Least Recently Used). The simulator takes input from a file that specifies a series of page requests and simulates the behavior of frames in memory as it handles these requests. The results are logged, showing how each algorithm manages page loads, hits, and evictions.

### Compilation-Execution
Compile the program using the GNU C++ Compiler with the following command.

`g++ PRAS.cpp -o <exe_name>`.

Once the program is ready for execution, the following command can be used to run the program:

`<executable_path> <algorithm> <num_frames> <swap_in_cost> <swap_out_cost> <input_file> <output_file>`

Where:
- **\<algorithm>** is either "FIFO" or "LRU."
- **<num_frames>** is the number of frames in memory.
- **<swap_in_cost>** and **<swap_out_cost>** are the costs associated with swapping pages in and out of memory.
- **<input_file>** is the path to the file containing the page references.
- **<output_file>** is the path where the output log will be written.

Upon completion, the output file will contain detailed logs of each page operation with each entry formatted as follows:

`<line_number>: <operation> <page_number> <result> <victim_page> <swap_in_time> <swap_out_time>`

Where:
- **\<operation>** is either 'R' for Read or 'W' for Write.
- **<page_number>** is the number of the page being referenced.
- **<result>** indicates whether the page was a 'H' for Hit (already in memory) or a 'F' for Fault (not in memory, required loading).
- **<victim_page>** shows the page number of any page that was evicted to make room for the current page. If no page was evicted, this is -1.
- **<swap_in_time>** and **<swap_out_time>** are the time units taken to swap a page into and out of memory, respectively.

You can view the output log with:

`cat <output_file>`

The standard output will also display a summary of the simulation results, including the total number of page references and faults, along with total swap times.

> [!CAUTION]
> I do not support people plagiarizing my code. I do not take responsibility for the unlawful actions of others.
