#include <iostream>
#include <string>
#include <fstream>
#include <vector>

#define ROOT 1
#define TABLE 2

//#define _debug_

class File{
public:
	char data[512];

	char* name = &data[0];
	uint64_t* flags = (uint64_t*)(&data[256]);
	uint32_t* prev_sec = (uint32_t*)(&data[264]);
	uint32_t* next_sec = (uint32_t*)(&data[268]);
	uint32_t* parent_sec = (uint32_t*)(&data[272]);
	uint64_t* size_in_sec = (uint64_t*)(&data[276]);
	uint64_t* size_in_bytes = (uint64_t*)(&data[284]);
	uint32_t* data_sec = (uint32_t*)(&data[292]);

	void setname(std::string name) {
		for (int i = 0; i < name.size(); i++) {
			data[i] = name[i];
		}
	}
};

uint32_t findSec(std::fstream* disk, uint32_t map_size) {
	char* map = new char[map_size];

	disk->seekg(TABLE*512);
	disk->read(map, map_size);

	for (int i = 0; i < map_size; i++) {
		if (map[i] != -1) {

			for (int j = 0; j < 8; j++) {
				if (!(map[i] & (1 << j))) {
					map[i] |= (1 << j);

					disk->seekg(TABLE * 512);
					disk->write(map, map_size);
					delete[] map;
					return i*8 + j;
				}
			}
		}
	}

	return 0;
}

uint32_t findFile(std::fstream disk, std::string name, uint32_t parent) {
	return 0;
}

int instal(std::string path, std::string name, std::fstream* data, uint64_t flags, std::fstream* disk) {
	int map_size;
	disk->seekg(35);
	disk->read((char*)(&map_size), sizeof(int));

	File file;

	std::string endName(path);
	endName += name;
	file.setname(endName);

	*file.flags = flags;

	data->seekg(0, data->end);
	*file.size_in_bytes = data->tellg();
	data->seekg(0, data->beg);

	*file.size_in_sec = std::ceil((double)(*file.size_in_bytes) / 512);

	std::vector<std::string> paths;

	for (int i = 0; i < path.size(); i++) {
		static std::string temp;

		if (path[i] == '\\') {
			paths.push_back(temp);
			temp.clear();
		}
		else {
			temp += path[i];
		}
	}

	if (paths.size() == 1) {

		*file.parent_sec = ROOT;
		
		char* buffer = new char[512];
		disk->seekg(ROOT * 512);
		disk->read(buffer, 512);

		*file.prev_sec = 0xFFFFFFFF;
		*file.next_sec = buffer[292];

		uint32_t fileheader = findSec(disk, map_size);
		buffer[292] = fileheader;
		disk->seekg(ROOT * 512);
		disk->write(buffer, 512);

		disk->seekg(*file.next_sec * 512);
		disk->read(buffer, 512);
		buffer[264] = fileheader;
		disk->seekg(*file.next_sec * 512);
		disk->write(buffer, 512);

		*file.data_sec = findSec(disk, map_size);

		disk->seekg(fileheader * 512);
		disk->write(file.data, 512);

		int file_size = *file.size_in_bytes;
		for (int i = 0; i < *file.size_in_sec; i++) {
			uint32_t address = findSec(disk, map_size);

			disk->seekg(*file.data_sec * 512 + 4 * i);
			disk->write((char*)(&address), 4);

			disk->seekg(address * 512);
			data->seekg(512 * i);
			data->read(buffer, (file_size <= 512) ? file_size : 512);
			disk->write(buffer, (file_size <= 512) ? file_size : 512);

			if (file_size > 512) {
				file_size -= 512;
			}
		}

		delete[] buffer;
	}

	return 0;
}

//INST root\ kernel.bin D:\OS\compiled\kernel.bin D:\OS\os.img
int instalCNSL() {

	std::vector<std::string> commands;
	std::string parsed;

	while (true) {
		std::cin >> parsed;
		commands.push_back(parsed);

		if (commands[0] == "INST") {

			for (int i = 0; i < 4; i++) {
				parsed.clear();
				std::cin >> parsed;

				commands.push_back(parsed);
			}

			std::fstream file(commands[3], std::fstream::in | std::fstream::binary);
			std::fstream disk(commands[4], std::ios::in | std::ios::out | std::fstream::binary);

			if (!file.is_open()) {
				std::cout << "Can't open file: " << commands[3] << std::endl;
				system("pause");
				return 0;
			}
			if (!disk.is_open()) {
				std::cout << "Can't open disk: " << commands[4] << std::endl;
				system("pause");
				return 0;
			}


			instal(commands[1], commands[2], &file, 0, &disk);

			file.close();
			disk.close();
		}
		else if (commands[0] == "HELP") {
			std::cout <<
				"INST <path> <name> <file> <disk> \n"
				"EXIT\n"
				<< std::endl;
		}
		else if (commands[0] == "EXIT") {
			return 0;
		}

		commands.clear();
		parsed.clear();
	}
}

int init(std::fstream* boot, std::fstream* disk, int size, std::fstream* bootcfg) {

	char* buffer = new char[512];

	boot->read(buffer, 512);
	disk->write(buffer, 512);

	File root;

	for (int i = 0; i < 512; i++) {
		root.data[i] = 0;
	}

	root.setname("root");
	*root.flags = 0;
	*root.prev_sec = 0xFFFFFFFF;
	*root.next_sec = 0xFFFFFFFF;
	*root.parent_sec = 0xFFFFFFFF;
	*root.size_in_bytes = 0;
	*root.size_in_sec = 0;
	*root.data_sec = 0xFFFFFFFF;

	disk->write(root.data, 512);

	int size_of_map_in_sec = std::ceil((double)size / 512);					//1 sector = 1 bit
	int size_of_map_in_bytes = std::ceil((double)(size_of_map_in_sec) / 8); //8 sectors = 1 byte
	int size_of_map = std::ceil((double)(size_of_map_in_bytes) / 512);		//physical size of map in clusters
	char* map = new char[size_of_map_in_bytes];
	
	for (int i = 0; i < size_of_map_in_bytes; i++) {
		map[i] = 0;
	}

	for (int i = 0; i < size_of_map + 2;) {
		for (int offset = 0; offset < 8 && i < size_of_map + 2; i++, offset++) {
			map[i/8] |= (1 << i);
		}
	}

	disk->write(map, size_of_map_in_bytes);

	delete[] map;

	for (int i = disk->tellg(); i < size; i++) {
		const char t = 0;
		disk->write(&t, 1);
	}

	disk->seekg(27);
	char t = 4;
	disk->write(&t, 1);
	disk->seekg(35);
	disk->write((char*)(&size_of_map_in_bytes), sizeof(int));



	uint32_t bootbinad = findSec(disk, size_of_map_in_bytes);
	File bootbin;
	bootbin.setname("boot.bin");
	*bootbin.flags = 0;
	*bootbin.prev_sec = 0xFFFFFFFF;
	*bootbin.next_sec = 0xFFFFFFFF;
	*bootbin.parent_sec = ROOT;
	*bootbin.size_in_bytes = 0x275;
	*bootbin.size_in_sec = std::ceil((double)(*bootbin.size_in_bytes) / 512);
	*bootbin.data_sec = findSec(disk, size_of_map_in_bytes);
	disk->seekg(bootbinad * 512);
	disk->write(bootbin.data, 512);

	disk->seekg(ROOT * 512 + 0x124);
	disk->write((char*)(&bootbinad), sizeof(uint32_t));

	for (int i = 0; i < *bootbin.size_in_sec; i++) {
		uint32_t address = findSec(disk, size_of_map_in_bytes);
		
		disk->seekg(*bootbin.data_sec*512 + 4 * i);
		disk->write((char*)(&address), 4);

		disk->seekg(address * 512);
		boot->seekg(512 * i + 512);
		boot->read(buffer, 512);
		disk->write(buffer, 512);
	}

	instal("root\\", "boot.cfg", bootcfg , 0, disk);

	std::cout << "INITALIZED" << std::endl;

	delete[] buffer;
	return 0;
}

int main(int count, char* argv[]) {

	if (count == 1) {
		std::cout << "INIT <boot.bin> <disk> <size> <boot.cfg> \n"
					 "INST <path> <name> <file> <disk> \n"
					 "INSTCNSL\n"
					 "EXIT\n"
				  << std::endl;
	}
	else {

		if (std::string(argv[1]) == "INST") {

			std::fstream file(argv[4], std::fstream::in | std::fstream::binary);
			std::fstream disk(argv[5], std::ios::in | std::ios::out | std::fstream::binary);

			if (!file.is_open()) {
				std::cout << "Can't open file: " << argv[4] << std::endl;
				system("pause");
				return 0;
			}
			if (!disk.is_open()) {
				std::cout << "Can't open disk: " << argv[5] << std::endl;
				system("pause");
				return 0;
			}


			instal(argv[2], argv[3], &file, 0, &disk);

			file.close();
			disk.close();
		}
		else if (std::string(argv[1]) == "INIT") {

			std::fstream file(argv[2], std::fstream::in | std::fstream::binary);
			std::fstream disk(argv[3], std::fstream::out | std::fstream::in | std::fstream::binary);
			std::fstream cfg(argv[5], std::fstream::in | std::fstream::binary);

			if (!file.is_open()) {
				std::cout << "Can't open file: " << argv[2] << std::endl;
				system("pause");
				return 0;
			}
			if (!disk.is_open()) {
				std::cout << "Can't open disk: " << argv[3] << std::endl;
				system("pause");
				return 0;
			}
			if (!cfg.is_open()) {
				std::cout << "Can't open cfg file: " << argv[5] << std::endl;
				system("pause");
				return 0;
			}

			init(&file, &disk, std::stoi(std::string(argv[4])), &cfg);

			file.close();
			disk.close();
			cfg.close();
		}
		else if (std::string(argv[1]) == "INSTCNSL") {
			instalCNSL();
		}
	}

	system("pause");
	return 0;
}