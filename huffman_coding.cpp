#include<iostream>

#include<fstream>

#include<queue>

#include<map>

#include<string>

#include<bitset>

#include <sstream>


using namespace std;

int  original_size,compressed_size;
/*
1)BuildHuffmanTree function: takes a map of character frequencies as input and builds the Huffman tree using a priority queue.

2)GenerateHuffmanCode function: recursively traverses the Huffman tree and generates Huffman codes for each character. It stores the codes in the codes map.

3)WriteHuffmanTreeToFile function : recursively writes the Huffman tree to a file. It uses 'L' to represent leaf nodes and 'I' to represent internal nodes.

4)ReadHuffmanTreeFromFile function : reads the Huffman tree from a file and reconstructs it. It uses 'L' and 'I' markers to determine leaf and internal nodes.

5)CompressText function : compresses the input text using Huffman coding. It first calculates the frequencies of each character in the text, builds the Huffman tree, and generates Huffman codes. Then it converts the text into compressed binary form and writes it, along with the Huffman tree, to a compressed file.

6)DecompressText function : decompresses the text from a compressed file. It reads the Huffman tree from the file, reads the compressed binary data, and performs the decoding process using the Huffman tree to obtain the original text. The decompressed text is then written to a file.

*/

class Node {

public:

    char data;

    int frequency;

    Node* left;

    Node* right;


    Node(char d, int f) : data(d), frequency(f), left(nullptr), right(nullptr) {}

};


class Compare {

public:

    bool operator()(Node* a, Node* b) {

        return a->frequency > b->frequency;

    }

};


class HuffmanCoding {

private:

    Node* root;

    map<char, string> codes;

    

    Node* BuildHuffmanTree(map<char, int> freq) {

        priority_queue<Node*, vector<Node*>, Compare> p;

        for (auto it = freq.begin(); it != freq.end(); it++) {

            Node* x = new Node(it->first, it->second);

            p.push(x);

        }


        while (p.size() > 1) {

            Node* left = p.top();

            p.pop();

            Node* right = p.top();

            p.pop();


            Node* temp = new Node('\0', left->frequency + right->frequency);

            temp->left = left;

            temp->right = right;


            p.push(temp);

        }


        return p.top();

    }


    void GenerateHuffmanCode(Node* node, const string& code) {

        if (!node) {

            return;

        }


        if (!node->left && !node->right) {

            codes[node->data] = code;

            return;

        }


        GenerateHuffmanCode(node->left, code + "0");

        GenerateHuffmanCode(node->right, code + "1");

    }


    void WriteHuffmanTreeToFile(fstream& outputFile, Node* node) {

        if (!node) {

            return;

        }


        if (!node->left && !node->right) {

            outputFile.put('L');

            outputFile.put(node->data);

        }

        else {

            outputFile.put('I');

            WriteHuffmanTreeToFile(outputFile, node->left);

            WriteHuffmanTreeToFile(outputFile, node->right);

        }

    }


    Node* ReadHuffmanTreeFromFile(fstream& inputFile) {

        char node_type;

        inputFile.get(node_type);


        if (node_type == 'L') {

            char data;

            inputFile.get(data);

            return new Node(data, 0);

        }

        else if (node_type == 'I') {

            Node* left = ReadHuffmanTreeFromFile(inputFile);

            Node* right = ReadHuffmanTreeFromFile(inputFile);


            Node*parent=new Node('\0', 0);

            parent->left=left;

            parent->right=right;

            return parent;

        }


        return nullptr;

    }


public:

    HuffmanCoding() {

        root = nullptr;

    }


    void CompressText(const string& text, const string& compressed_file_path) {

        map<char, int> freq;

        for (char c : text) {

            freq[c]++;

        }
        

        root = BuildHuffmanTree(freq);
        
        GenerateHuffmanCode(root, "");
        
        /*for(auto it:codes)
        {
            cout<<it.first<<" "<<it.second<<endl;
        }*/

        string compressed_data;

        for (char c : text) {

            compressed_data += codes[c];

        }
         int total_size=(int)compressed_data.length();
         
        int OFF_SET=total_size%8;
        
        for(int i=0;i<8-OFF_SET;i++)
        {
            compressed_data+='0';
        }
        
         /*for(int i=0;i<compressed_data.length();i++)
        {
            cout<<compressed_data[i];
        }*/
      
        string compressed_bytes;
        
        for (size_t i = 0; i < compressed_data.length(); i += 8) {

            bitset<8> bits(compressed_data.substr(i, 8));

            compressed_bytes += char(bits.to_ulong());
            

        }
        
        

        fstream compressed_file(compressed_file_path, ios::out | ios::binary);

        if (compressed_file.is_open()) {
        
            WriteHuffmanTreeToFile(compressed_file, root);
            compressed_file.put(OFF_SET);

            compressed_file << compressed_bytes;

            compressed_file.close();

            cout << "Compressed data written to file: " << compressed_file_path << endl;

        }

        else {

            cout << "Failed to open the compressed file." << endl;

        }

    }


    void DecompressText(const string& compressed_file_path, const string& decompressed_file_path) {

        fstream compressed_file(compressed_file_path, ios::in|ios::binary);

        if (!compressed_file.is_open()) {

            cout << "Failed to open the compressed file." << endl;

            return;

        }

       
        
        root = ReadHuffmanTreeFromFile(compressed_file);
        
        string compressed_data;

        ostringstream s;

        s << compressed_file.rdbuf();

        compressed_data = s.str();

        compressed_file.close();
        int OFF_SET=compressed_data[0];
        
        string bit_string;
      
        for (size_t i=1;i<compressed_data.length();i++) {
              char c=compressed_data[i];
              bit_string += bitset<8>(c).to_string();
        }
        int total_size=(int)bit_string.length()-(8-OFF_SET);
        compressed_size=total_size;
        
        /*for(int i=0;i<total_size;i++)
        {
            
            cout<<bit_string[i];
        }*/
        
        map<string,char>reverse_codes;
        for (auto pair : codes) {

            reverse_codes[pair.second] = pair.first;

        }


        string decoded_text, current_code;
        
        
        for (int i=0;i<total_size;i++) {

            current_code += bit_string[i];

            if(reverse_codes.find(current_code)!=reverse_codes.end()) {

                decoded_text += reverse_codes[current_code];

                current_code = "";

            }

        }
        
        

        fstream decompressed_file(decompressed_file_path, ios::out | ios::binary);

        if (decompressed_file.is_open()) {

            decompressed_file << decoded_text;

            decompressed_file.close();

            cout << "Decompressed data written to file: " << decompressed_file_path << endl;

        }

        else {

            cout << "Failed to open the decompressed file." << endl;

        }

    }

};


int main() {

    cout<<"give a path of the file to be compressed:"<<endl;
    string file_path;
    cin>>file_path;
    fstream file(file_path, ios::in);

    if (!file.is_open()) {

        cout << "Unable to open the file" << endl;

        return 0;

    }


    string text;
    
    ostringstream s;

    s << file.rdbuf();

    text = s.str();
    
    file.close();
    
    original_size=text.size();
    for(int i=1;i<=4;i++)
    {
         file_path.pop_back();
    }

    HuffmanCoding huffman;

    huffman.CompressText(text, file_path+"_compressed.txt");

    huffman.DecompressText(file_path+"_compressed.txt", file_path+"_decompressed.txt");

    float compression_ratio=float(compressed_size)/(original_size*8);
    cout<<"compression ratio is :"<<compression_ratio<<endl;
    return 0;

}

