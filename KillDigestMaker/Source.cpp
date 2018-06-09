#include <array>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include <filesystem>

#include "opencv2/opencv.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/core.hpp"

#pragma comment(lib, "C:\\opencv3.4.1\\build\\x64\\vc14\\lib\\opencv_world341.lib")
#pragma warning(disable : 4996)

namespace fs = std::experimental::filesystem;

std::string target_movie_directory = "TargetFiles"; // 動画の入っているフォルダー（作ってね）


bool ExecCmd(const char* cmd, std::string& stdOut, int& exitCode) {
	std::shared_ptr<FILE> pipe(_popen(cmd, "r"), [&](FILE* p) {exitCode = _pclose(p); });
	if (!pipe) {
		return false;
	}
	std::array<char, 256> buf;
	while (!feof(pipe.get())) {
		if (fgets(buf.data(), buf.size(), pipe.get()) != nullptr) {
			stdOut += buf.data();
		}
	}
	return true;
}

void command(std::string cmd) {
	std::string stdOut;
	int exitCode;
	if (ExecCmd(cmd.c_str(), stdOut, exitCode)) {
		std::cout << stdOut << std::endl;
	}
	else {
		std::cout << "標準出力の取得に失敗しました。" << std::endl;
	}
}

int main() {

	fs::remove_all(fs::path("output"));

	std::vector<std::string> target_files;
	for (auto ent : fs::recursive_directory_iterator(target_movie_directory)) {
		std::cout << ent << std::endl;
		std::stringstream ss;
		ss << ent;
		target_files.push_back(ss.str());
	}

	fs::create_directory(fs::path("output"));
	std::ofstream ofs("output/files.txt");

	int output_file_count = 0;
	for (int target_file_index = 0; target_file_index < target_files.size(); target_file_index++) {

		std::string target_file = target_files[target_file_index];
		std::cout << target_file << std::endl;

		cv::VideoCapture cap(target_file);
		float FPS = cap.get(CV_CAP_PROP_FPS);

		float global_scale = 0.25 / cap.get(CV_CAP_PROP_FRAME_HEIGHT) * 1080.f;

		int frame_num = cap.get(CV_CAP_PROP_FRAME_COUNT);
		float temp_scale = cap.get(CV_CAP_PROP_FRAME_HEIGHT) / 1080.f;

		cv::Mat temp = cv::imread("template.jpg");
		cv::resize(temp, temp, cv::Size(), global_scale * temp_scale, global_scale * temp_scale);

		std::queue<int> key_frames;

		cv::Mat frame;
		for (int i = 0; i < frame_num; i += FPS) {
			cap.set(CV_CAP_PROP_POS_FRAMES, i);
			cap >> frame;
			std::cout << "Searching kill frame... " << i * 100 / (float)frame_num << "%";
			cv::Mat frame_rect(frame, cv::Rect(frame.cols / 5 * 2, frame.rows / 9 * 8, frame.cols / 5, frame.rows / 9));
			cv::Mat frame_resize;
			cv::resize(frame_rect, frame_resize, cv::Size(), global_scale, global_scale);
			cv::Mat result;
			cv::matchTemplate(frame_resize, temp, result, CV_TM_CCORR_NORMED);
			cv::Point pt;
			double max_value;
			cv::minMaxLoc(result, NULL, &max_value, NULL, &pt);
			if (max_value > 0.80) {
				key_frames.push(i);
				std::cout << "  Found";
			}
			std::cout << std::endl;
		}

		std::vector<std::vector<int>> key_frames_grouped;
		std::vector<int> local_key_frames;
		int prev_value = key_frames.front();
		while (!key_frames.empty()) {
			if (abs(prev_value - key_frames.front()) > FPS * 3) {
				key_frames_grouped.push_back(std::vector<int>(local_key_frames));
				local_key_frames.clear();
			}
			local_key_frames.push_back(key_frames.front());
			prev_value = key_frames.front();
			key_frames.pop();
		}
		key_frames_grouped.push_back(std::vector<int>(local_key_frames));
		for (int i = 0; i < key_frames_grouped.size(); i++) {
			if (key_frames_grouped[i].size() < 5) continue;
			for (int j = 0; j < key_frames_grouped[i].size(); j++) {
				std::cout << key_frames_grouped[i][j] << "\t";
			}
			std::cout << std::endl;
		}
		cap.release();
		
		for (int i = 0; i < key_frames_grouped.size(); i++) {
			if (key_frames_grouped[i].size() < 5) continue;
			command("ffmpeg -ss " + std::to_string(key_frames_grouped[i][0] / FPS - 2.5)
				+ " -i \"" + target_file + "\" -t " + std::to_string((key_frames_grouped[i][key_frames_grouped[i].size() - 1] - key_frames_grouped[i][0]) / FPS)
				+ " -vcodec hevc output/" + std::to_string(output_file_count) + ".mp4");
			ofs << "file \'" + std::to_string(output_file_count) + ".mp4\'" << std::endl;
			output_file_count++;
		}
	}

	time_t t = time(NULL);
	struct tm tm;
	char str[81];

	localtime_s(&tm, &t);
	strftime(str, sizeof(str), "%Y%m%d-%H%M%S", &tm);

	std::string output_file_name(str);

	command("ffmpeg -safe 0 -vcodec hevc -f concat -i output/files.txt " + output_file_name + ".mp4");
	system("pause");
	return 0;
}

