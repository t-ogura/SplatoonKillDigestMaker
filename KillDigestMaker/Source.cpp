#include <array>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include <filesystem>
#include <Windows.h>

#include "opencv2/opencv.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/core.hpp"

#pragma comment(lib, "opencv_world341.lib")
#pragma warning(disable : 4996)

namespace fs = std::experimental::filesystem;

std::string target_movie_directory = "TargetFiles"; // 動画の入っているフォルダー（作ってね）

class Parameters {
public:
	Parameters() {
		ifs.open("init.txt");
		std::string line;
		param_init();
		while (std::getline(ifs, line)) {
			if (line.size() < 1) continue;
			if (line[0] == '%') continue;
			auto pos = line.find("%");
			if (pos < std::string::npos) {
				line.erase(pos, line.size() - pos);
			}
			std::vector < std::string> splited_line = split(line, ' ');
			if (splited_line.size() > 1) {
				if (splited_line[0] == "TAMPLATE_MATCHING_THRESHOLD") {
					TAMPLATE_MATCHING_THRESHOLD = std::atof(splited_line[1].c_str());
				}
				else if (splited_line[0] == "ADDITIONAL_TIME_BEFORE_KILL") {
					ADDITIONAL_TIME_BEFORE_KILL = std::atof(splited_line[1].c_str());
				}
				else if (splited_line[0] == "ADDITIONAL_TIME_AFTER_KILL") {
					ADDITIONAL_TIME_AFTER_KILL = std::atof(splited_line[1].c_str());
				}
				else if (splited_line[0] == "CONNECTING_INTERVAL") {
					CONNECTING_INTERVAL = std::atof(splited_line[1].c_str());
				}
				else if (splited_line[0] == "CANDIDATE_FRAME_NUM") {
					CANDIDATE_FRAME_NUM = std::atof(splited_line[1].c_str());
				}
				else if (splited_line[0] == "SEARCHING_FRAME_INTERVAL") {
					SEARCHING_FRAME_INTERVAL = std::atof(splited_line[1].c_str());
				}
				else if (splited_line[0] == "CUT_OUT_ENCORD") {
					CUT_OUT_ENCORD = std::atoi(splited_line[1].c_str());
				}
				else if (splited_line[0] == "OUTPUT_ENCORD") {
					OUTPUT_ENCORD = std::atoi(splited_line[1].c_str());
				}
				else if (splited_line[0] == "CUT_OUT_IGNORE") {
					CUT_OUT_IGNORE = std::atoi(splited_line[1].c_str());
				}
				else if (splited_line[0] == "OUTPUT_IGNORE") {
					OUTPUT_IGNORE = std::atoi(splited_line[1].c_str());
				}
				else if (splited_line[0] == "MOVIE_SIZE_DESIGNATION") {
					MOVIE_SIZE_DESIGNATION = std::atoi(splited_line[1].c_str());
				}
				else if (splited_line[0] == "MOVIE_SIZE_HEIGHT") {
					MOVIE_SIZE_HEIGHT = std::atoi(splited_line[1].c_str());
				}
				else if (splited_line[0] == "WIN_DETECTION") {
					WIN_DETECTION = std::atoi(splited_line[1].c_str());
				}
				else if (splited_line[0] == "LOSE_DETECTION") {
					LOSE_DETECTION = std::atoi(splited_line[1].c_str());
				}
			}
		}
		display_parameters();
		ifs.close();
	}

	float TAMPLATE_MATCHING_THRESHOLD;
	float ADDITIONAL_TIME_BEFORE_KILL;
	float ADDITIONAL_TIME_AFTER_KILL;
	float CONNECTING_INTERVAL;
	float CANDIDATE_FRAME_NUM;
	float SEARCHING_FRAME_INTERVAL;
	int CUT_OUT_ENCORD;
	int OUTPUT_ENCORD;
	int CUT_OUT_IGNORE;
	int OUTPUT_IGNORE;
	int MOVIE_SIZE_DESIGNATION;
	int MOVIE_SIZE_HEIGHT;
	int WIN_DETECTION;
	int LOSE_DETECTION;

private:
	std::ifstream ifs;

	template<class T> std::vector<std::string> split(const std::string& s, const T& separator, bool ignore_empty = 0, bool split_empty = 0) {
		struct {
			auto len(const std::string&             s) { return s.length(); }
			auto len(const std::string::value_type* p) { return p ? std::char_traits<std::string::value_type>::length(p) : 0; }
			auto len(const std::string::value_type  c) { return c == std::string::value_type() ? 0 : 1; /*return 1;*/ }
		} util;

		if (s.empty()) { /// empty string ///
			if (!split_empty || util.len(separator)) return{ "" };
			return{};
		}

		auto v = std::vector<std::string>();
		auto n = static_cast<std::string::size_type>(util.len(separator));
		if (n == 0) {    /// empty separator ///
			if (!split_empty) return{ s };
			for (auto&& c : s) v.emplace_back(1, c);
			return v;
		}

		auto p = std::string::size_type(0);
		while (1) {      /// split with separator ///
			auto pos = s.find(separator, p);
			if (pos == std::string::npos) {
				if (ignore_empty && p - n + 1 == s.size()) break;
				v.emplace_back(s.begin() + p, s.end());
				break;
			}
			if (!ignore_empty || p != pos)
				v.emplace_back(s.begin() + p, s.begin() + pos);
			p = pos + n;
		}
		return v;
	}

	void param_init() {
		TAMPLATE_MATCHING_THRESHOLD = 0.80;
		ADDITIONAL_TIME_BEFORE_KILL = 2.5;
		ADDITIONAL_TIME_AFTER_KILL = 0.0;
		CONNECTING_INTERVAL = 4.0;
		CANDIDATE_FRAME_NUM = 5;
		SEARCHING_FRAME_INTERVAL = 1.0;
		CUT_OUT_ENCORD = 1;
		OUTPUT_ENCORD = 1;
		CUT_OUT_IGNORE = 0;
		OUTPUT_IGNORE = 0;
		MOVIE_SIZE_DESIGNATION = 0;
		MOVIE_SIZE_HEIGHT = 1080;
		WIN_DETECTION = 0;
		LOSE_DETECTION = 0;
	}

	void display_parameters() {
		std::cout <<
			"TAMPLATE_MATCHING_THRESHOLD\t" << TAMPLATE_MATCHING_THRESHOLD << std::endl <<
			"ADDITIONAL_TIME_BEFORE_KILL\t" << ADDITIONAL_TIME_BEFORE_KILL << std::endl <<
			"ADDITIONAL_TIME_AFTER_KILL\t" << ADDITIONAL_TIME_AFTER_KILL << std::endl <<
			"CONNECTING_INTERVAL\t" << CONNECTING_INTERVAL << std::endl <<
			"CANDIDATE_FRAME_NUM\t" << CANDIDATE_FRAME_NUM << std::endl <<
			"SEARCHING_FRAME_INTERVAL\t" << SEARCHING_FRAME_INTERVAL << std::endl <<
			"CUT_OUT_ENCORD\t" << CUT_OUT_ENCORD << std::endl <<
			"OUTPUT_ENCORD\t" << OUTPUT_ENCORD << std::endl <<
			"CUT_OUT_IGNORE\t" << CUT_OUT_IGNORE << std::endl <<
			"OUTPUT_IGNORE\t" << OUTPUT_IGNORE << std::endl <<
			"MOVIE_SIZE_DESIGNATION\t" << MOVIE_SIZE_DESIGNATION << std::endl <<
			"MOVIE_SIZE_HEIGHT\t" << MOVIE_SIZE_DESIGNATION << std::endl <<
			"WIN_DETECTION\t" << MOVIE_SIZE_DESIGNATION << std::endl <<
			"LOSE_DETECTION\t" << MOVIE_SIZE_HEIGHT << std::endl;
	}
};

void command(std::string cmd) {
	std::ofstream ofs("bat.bat");
	ofs << cmd << std::endl;
	ofs.close();
	system(".\\bat.bat");
}

std::string replace_all(std::string s, std::string target, std::string replacement) {

	if (!target.empty()) {
		std::string::size_type pos = 0;
		while ((pos = s.find(target, pos)) != std::string::npos) {
			s.replace(pos, target.length(), replacement);
			pos += replacement.length();
		}
	}
	return s;
}

void make_py_file(std::string video_file, double A, double B) {

	std::ofstream ofs("avidemux_project.py");
	ofs << "#PY  <- Needed to identify #" << std::endl;
	ofs << "#--automatically built--" << std::endl;
	ofs << "" << std::endl;
	ofs << "adm = Avidemux()" << std::endl;
	std::string video_abs_path = fs::absolute(fs::path(video_file)).string();
	ofs << "adm.loadVideo(\"" << replace_all(replace_all(video_abs_path, "\\", "/"), "_", "\\_") << "\")" << std::endl;
	ofs << "adm.clearSegments()" << std::endl;
	ofs << "adm.videoCodec(\"Copy\")" << std::endl;
	ofs << "adm.audioClearTracks()" << std::endl;
	ofs << "adm.setSourceTrackLanguage(0,\"unknown\")" << std::endl;
	ofs << "adm.audioAddTrack(0)" << std::endl;
	ofs << "adm.audioCodec(0, \"copy\");" << std::endl;
	ofs << "adm.audioSetDrc(0, 0)" << std::endl;
	ofs << "adm.audioSetShift(0, 0,0)" << std::endl;
	ofs << "adm.setContainer(\"MP4\", \"muxerType=0\", \"useAlternateMp3Tag=True\")" << std::endl;
	ofs << "adm.addSegment(0, 0, " << std::to_string(10000000000000000) << ")" << std::endl;
	ofs << "adm.markerA = " << std::to_string((int64)(A * 1000000)) << std::endl;
	ofs << "adm.markerB = " << std::to_string((int64)(B * 1000000)) << std::endl;
}


int main() {

	Parameters params = Parameters();

	if (params.CUT_OUT_IGNORE != 1) {

		std::vector < std::vector< std::string > > target_folders;
		std::vector<std::string> temp_target_files;
		for (auto ent : fs::directory_iterator(target_movie_directory)) {
			std::stringstream ss;
			ss << ent;
			if (fs::is_directory(ent)) {
				std::vector<std::string> local_vector;
				for (auto local_ent : fs::recursive_directory_iterator(ent)) {
					//std::cout << local_ent << std::endl;
					if (!fs::is_directory(local_ent)) {
						std::stringstream local_ss;
						local_ss << local_ent;
						local_vector.push_back(local_ss.str());
					}
				}
				if (local_vector.size() > 0) {
					target_folders.push_back(local_vector);
				}
			}
			else {
				temp_target_files.push_back(ss.str());
			}
		}
		target_folders.push_back(temp_target_files);
		
		for (int x = 0; x < target_folders.size(); x++) {
			std::cout << "//  " << x << std::endl;
			for (int y = 0; y < target_folders[x].size(); y++) {
				std::cout << target_folders[x][y] << std::endl;
			}
		}

		for (int target_folder_index = 0; target_folder_index < target_folders.size(); target_folder_index++) {
			std::vector<std::string> target_files = target_folders[target_folder_index];

			fs::remove_all(fs::path("output"));

			int max_height_size = 0;
			for (int i = 0; i < target_files.size(); i++) {
				std::string target_file = target_files[i];
				cv::VideoCapture cap(target_file);
				if (max_height_size < cap.get(CV_CAP_PROP_FRAME_HEIGHT)) {
					max_height_size = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
				}
				cap.release();
			}
			if (params.MOVIE_SIZE_DESIGNATION == 1) {
				max_height_size = params.MOVIE_SIZE_HEIGHT;
			}

			fs::create_directory(fs::path("output"));
			std::ofstream ofs("output/files.txt");

			int output_file_count = 0;
			for (int target_file_index = 0; target_file_index < target_files.size(); target_file_index++) {

				try {

					std::string target_file = target_files[target_file_index];
					std::string target_file_name(target_file);
					target_file_name.erase(0, target_file_name.rfind("\\")+1);
					target_file_name.erase(target_file_name.rfind("."), target_file_name.size()- target_file_name.rfind("."));
					std::cout << target_file << std::endl;

					cv::VideoCapture cap(target_file);
					if (!cap.isOpened()) continue;

					float FPS = cap.get(CV_CAP_PROP_FPS);

					int frame_num = cap.get(CV_CAP_PROP_FRAME_COUNT);
					float temp_scale = cap.get(CV_CAP_PROP_FRAME_HEIGHT) / 1080.f;

					cv::Mat temp = cv::imread("template.jpg");
					cv::Mat win_temp = cv::imread("win_template.jpg");
					cv::Mat lose_temp = cv::imread("lose_template.jpg");

					std::queue<int> key_frames;

					cv::Mat frame;
					for (int i = 0; i < frame_num; i++) {
						if (i % ((int)(FPS * params.SEARCHING_FRAME_INTERVAL)) != 0) {
							cap.grab();
							continue;
						}
						else {
							cap >> frame;
						}
						std::cout << "Searching kill frame... " << i * 100 / (float)frame_num << "%";

						try { // for kill digest
							cv::Mat frame_rect(frame, cv::Rect(frame.cols *(920.0 / 1920.0), frame.rows *(992.0 / 1080.0), frame.cols *(260.0 / 1920.0), frame.rows *(44.0 / 1080.0)));
							cv::Mat result;
							cv::matchTemplate(frame_rect, temp, result, CV_TM_CCORR_NORMED);
							cv::Point pt;
							double max_value;
							cv::minMaxLoc(result, NULL, &max_value, NULL, &pt);
							if (max_value > (params.TAMPLATE_MATCHING_THRESHOLD)) {
								key_frames.push(i);
								std::cout << "  Kill!!";
							}

						}
						catch (...) {

						}
						if (params.WIN_DETECTION == 1) {
							try { // for win detection
								cv::Mat frame_rect(frame, cv::Rect(frame.cols *(36.0 / 1920.0), frame.rows *(54.0 / 1080.0), frame.cols *(206.0 / 1920.0), frame.rows *(74.0 / 1080.0)));
								cv::Mat result;
								cv::matchTemplate(frame_rect, win_temp, result, CV_TM_CCORR_NORMED);
								cv::Point pt;
								double max_value;
								cv::minMaxLoc(result, NULL, &max_value, NULL, &pt);
								if (max_value > (params.TAMPLATE_MATCHING_THRESHOLD)) {
									key_frames.push(i);
									std::cout << "  Win!!";
								}

							}
							catch (...) {

							}

						}
						if (params.LOSE_DETECTION == 1) {
							try { // for win detection
								cv::Mat frame_rect(frame, cv::Rect(frame.cols *(32.0 / 1920.0), frame.rows *(64.0 / 1080.0), frame.cols *(268.0 / 1920.0), frame.rows *(68.0 / 1080.0)));
								cv::Mat result;
								cv::matchTemplate(frame_rect, lose_temp, result, CV_TM_CCORR_NORMED);
								cv::Point pt;
								double max_value;
								cv::minMaxLoc(result, NULL, &max_value, NULL, &pt);
								if (max_value > (params.TAMPLATE_MATCHING_THRESHOLD)) {
									key_frames.push(i);
									std::cout << "  Lose...";
								}

							}
							catch (...) {

							}

						}
						std::cout << std::endl;
					}

					std::vector<std::vector<int>> key_frames_grouped;
					std::vector<int> local_key_frames;
					if (key_frames.empty()) continue;
					int prev_value = key_frames.front();
					while (!key_frames.empty()) {
						if (abs(prev_value - key_frames.front()) > FPS * (params.CONNECTING_INTERVAL * params.SEARCHING_FRAME_INTERVAL)) {
							key_frames_grouped.push_back(std::vector<int>(local_key_frames));
							local_key_frames.clear();
						}
						local_key_frames.push_back(key_frames.front());
						prev_value = key_frames.front();
						key_frames.pop();
					}
					if (!local_key_frames.empty())
						key_frames_grouped.push_back(std::vector<int>(local_key_frames));
					if (key_frames_grouped.empty()) continue;
					for (int i = 0; i < key_frames_grouped.size(); i++) {
						if (key_frames_grouped[i].size() < (params.CANDIDATE_FRAME_NUM)) continue;
						for (int j = 0; j < key_frames_grouped[i].size(); j++) {
							std::cout << key_frames_grouped[i][j] << "\t";
						}
						std::cout << std::endl;
					}
					cap.release();

					for (int i = 0; i < key_frames_grouped.size(); i++) {
						if (key_frames_grouped[i].size() < (params.CANDIDATE_FRAME_NUM)) continue;
						if (params.CUT_OUT_ENCORD == 1) {
							command("ffmpeg -ss " + std::to_string(key_frames_grouped[i][0] / FPS - (params.ADDITIONAL_TIME_BEFORE_KILL))
								+ " -i \"" + target_file + "\" -t " + std::to_string((key_frames_grouped[i][key_frames_grouped[i].size() - 1] - key_frames_grouped[i][0]) / FPS + (params.ADDITIONAL_TIME_AFTER_KILL))
								+ " -vf scale=-1:" + std::to_string(max_height_size)
								+ " -vcodec hevc \"output/" + std::to_string(output_file_count) + "-" + target_file_name + ".mp4\"");
						}
						else {
							std::string python_file = fs::absolute(fs::path("avidemux_project.py")).string();
							make_py_file(target_file, key_frames_grouped[i][0] / FPS - (params.ADDITIONAL_TIME_BEFORE_KILL), key_frames_grouped[i][key_frames_grouped[i].size() - 1] / FPS + (params.ADDITIONAL_TIME_AFTER_KILL));

							command("avidemux_cli --run \"" + python_file + "\" --save \"output/" + std::to_string(output_file_count) + "-" + target_file_name + ".mp4\" --quit");

							//command("ffmpeg -ss " + std::to_string(key_frames_grouped[i][0] / FPS - (params.ADDITIONAL_TIME_BEFORE_KILL))
							//	+ " -i \"" + target_file + "\" -ss 0 -t " + std::to_string((key_frames_grouped[i][key_frames_grouped[i].size() - 1] - key_frames_grouped[i][0]) / FPS + (params.ADDITIONAL_TIME_AFTER_KILL))
							//	+ " -map 0 -c:v copy -c:a copy -async 1 -strict -2 \"output/" + std::to_string(output_file_count) + "-" + target_file_name + ".mp4\"");
						}
						ofs << "file \'" + std::to_string(output_file_count) + "-" + target_file_name + ".mp4\'" << std::endl;
						output_file_count++;
					}
				}
				catch (...) {

				}
			}

			if (params.OUTPUT_IGNORE != 1) {
				time_t t = time(NULL);
				struct tm tm;
				char str[81];

				localtime_s(&tm, &t);
				strftime(str, sizeof(str), "%Y%m%d-%H%M%S", &tm);

				std::string output_file_name(str);
				if (params.OUTPUT_ENCORD == 1) {//  -vcodec hevc
					command("ffmpeg -safe 0 -f concat -i output/files.txt " + output_file_name + ".mp4");
				}
				else {
					command("ffmpeg -safe 0 -f concat -i output/files.txt -c:v copy -c:a copy -map 0:v -map 0:a " + output_file_name + ".mp4");
				}
			}
		}
	}
	else if (params.OUTPUT_IGNORE != 1) {
		time_t t = time(NULL);
		struct tm tm;
		char str[81];

		localtime_s(&tm, &t);
		strftime(str, sizeof(str), "%Y%m%d-%H%M%S", &tm);

		std::string output_file_name(str);
		if (params.OUTPUT_ENCORD == 1) {//  -vcodec hevc
			command("ffmpeg -safe 0 -f concat -i output/files.txt " + output_file_name + ".mp4");
		}
		else {
			command("ffmpeg -safe 0 -f concat -i output/files.txt -c:v copy -c:a copy -map 0:v -map 0:a " + output_file_name + ".mp4");
		}
	}

	system("pause");
	return 0;
}
