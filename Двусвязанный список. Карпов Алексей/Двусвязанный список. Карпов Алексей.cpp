#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <memory>

struct ListNode {
	std::string data;
	ListNode* rand;
	ListNode(const std::string& d) : data(d), rand(nullptr) {}
};

int main() {
	const std::string inputFileName = "Inlet.in";
	const std::string outputFileName = "outlet.out";

	std::ifstream fin(inputFileName);
	if (!fin) {
		std::cerr << "Не удалось открыть файл " << inputFileName << std::endl;
		return 1;
	}

	// Создадим вектор для хранения данных каждого узла
	std::vector<std::string> nodeData;

	// Также создадим вектор для хранения индексов rand или -1
	std::vector<int> randIntices;

	std::string line;

	// Считываем строки файла Inlet.in
	while (std::getline(fin, line)) {
		size_t semicolonPos = line.find(';');
		if (semicolonPos == std::string::npos) {
			// Пропуск некорректных строк
			continue;
		}
		std::string dataPart = line.substr(0, semicolonPos);
		std::string indexPart = line.substr(semicolonPos + 1);

		// Удалим пробелы и возможные символы
		int randIdx = -1;
		try {
			randIdx = std::stoi(indexPart);
		}
		catch (...) {
			randIdx = -1;
		}

		nodeData.push_back(dataPart);
		randIntices.push_back(randIdx);
	}
	fin.close();

	size_t n = nodeData.size();
	if (n > 1000000) {
		std::cerr << "Превышено допустимое количество узлов." << std::endl;
		return 1;
	}

	// Создадим массив узлов
	std::vector<ListNode*> nodes(n, nullptr);

	// Создаем узлы
	for (size_t i = 0; i < n; ++i) {
		nodes[i] = new ListNode(nodeData[i]);
	}

	// Связываем узлы цепочкой
	for (size_t i = 0; i + 1 < n; ++i) {

		// Созздаем головной указатель
		ListNode* head = nullptr;
		if (n > 0) {
			head = nodes[0];
		}

		// Устанавливаем rand указатели
		for (size_t i = 0; i < n; ++i) {
			int randIdx = randIntices[i];
			if (randIdx >= 0 && static_cast<size_t>(randIdx) < n) {
				nodes[i]->rand = nodes[randIdx];
			}
			else {
				nodes[i]->rand = nullptr;
			}
		}

		// Сериализуем список в бинарный файл
		std::ofstream fout(outputFileName, std::ios::binary);
		if (!fout) {
			std::cerr << "Не удалось открыть файл " << outputFileName << " для записи." << std::endl;

			// Освобождаем все узлы
			for (auto node : nodes) delete node;
			return 1;
		}

		// Записываем общее количество узлов
		uint64_t nodeCount = n;
		fout.write(reinterpret_cast<const char*>(&nodeCount), sizeof(nodeCount));

		for (size_t i = 0; i < n; ++i) {

			// Записываем длину данных
			uint64_t dataLen = nodeData[i].size();
			fout.write(reinterpret_cast<const char*>(&dataLen), sizeof(dataLen));

			// Записываем данные
			fout.write(nodeData[i].c_str(), dataLen);

			// Записываем индекс rand-узла
			int randIdxWrite = randIntices[i];
			fout.write(reinterpret_cast<const char*>(&randIdxWrite), sizeof(randIdxWrite));
		}

		for (auto node : nodes) delete node;

		fout.close();

		return 0;
	}
}