#include <iostream>
#include <vector>
#include <fstream>
using namespace std;  

struct PopulationMeanVariance {
	double sum = 0;
	double average = 0;
	double SSx = 0;
	double sigma = 0;

	double confidence_interval_low = 0;
	double confidence_interval_high = 0;

	void show_std_results() {
		cout << "\n\n標本平均：" << average << endl;
		cout << "偏差平方和：" << SSx << endl;
		cout << "不偏分散：" << sigma << endl;
	}
	void show_confidence_interval() {
		cout << "推定区間 [" << confidence_interval_low << "～" << confidence_interval_high << "]\n";
	}
};

void std_results_calculation(vector<double> experiment_data, PopulationMeanVariance * common) {
	for (auto i = experiment_data.begin(); i != experiment_data.end(); ++i) {
		(*common).sum += *i;
	}
	(*common).average = (*common).sum / experiment_data.size();
	for (auto i = experiment_data.begin(); i != experiment_data.end(); ++i) {
		(*common).SSx += pow(*i, 2.0);
	}
	(*common).SSx -= pow((*common).sum, 2.0) / experiment_data.size();
	(*common).sigma = (*common).SSx / (experiment_data.size() - 1);
}

int confidence_interval_calculation(vector<double> experiment_data, 
	                                int confidence_interval,
	                                PopulationMeanVariance* common,
	                                PopulationMeanVariance* mean, 
	                                PopulationMeanVariance* variance) {
	//t分布表ファイルの読み込み
	ifstream t_kentei_data_infile("t_kentei.txt");
	//t分布表ファイルの読み込みチェック
	if (!t_kentei_data_infile) {
		cout << "t分布表ファイルが見つかりません。\n";
		return 2;
	}
	//t分布表の格納
	double temp = 0;
	vector<double> t_kentei_data;
	while (t_kentei_data_infile >> temp) {
		t_kentei_data.push_back(temp);
	}
	t_kentei_data_infile.close();

	//カイ二乗分布表の読み込み
	ifstream kai_kentei_data_infile("kai_kentei.txt");
	//カイ二乗分布表ファイルの読み込みチェック
	if (!kai_kentei_data_infile) {
		cout << "カイ二乗分布表ファイルが見つかりません。\n";
		return 3;
	}
	//カイ二乗分布表の格納
	temp = 0;
	vector<double> kai_kentei_data;
	while (kai_kentei_data_infile >> temp) {
		kai_kentei_data.push_back(temp);
	}
	kai_kentei_data_infile.close();
	//分布表から値を抽出
	double t = 0;
	double x = 1;
	double xx = 1;
	switch (confidence_interval) {
	case 90:
		t  = t_kentei_data[(experiment_data.size() - 2) * 3];
		x  = kai_kentei_data[(experiment_data.size() - 2) * 6 + 3];
		xx = kai_kentei_data[(experiment_data.size() - 2) * 6 + 2];
		break;
	case 95:
        t  = t_kentei_data[(experiment_data.size() - 2) * 3 + 1];
		x  = kai_kentei_data[(experiment_data.size() - 2) * 6 + 4];
		xx = kai_kentei_data[(experiment_data.size() - 2) * 6 + 1];
		break;
	case 98:
	    t  = t_kentei_data[(experiment_data.size() - 2) * 3 + 2];
		x  = kai_kentei_data[(experiment_data.size() - 2) * 6 + 5];
		xx = kai_kentei_data[(experiment_data.size() - 2) * 6];
		break;
	}
	//母平均の計算
	(*mean).confidence_interval_low  = (*common).sum / experiment_data.size() - t * sqrt((*common).sigma / experiment_data.size());
	(*mean).confidence_interval_high = (*common).sum / experiment_data.size() + t * sqrt((*common).sigma / experiment_data.size());
	//母分散の計算
	(*variance).confidence_interval_low  = (experiment_data.size() - 1) * (*common).sigma / x;
	(*variance).confidence_interval_high = (experiment_data.size() - 1) * (*common).sigma / xx;
	return 0;
}

int main(){
	cout << "読み込んだファイルのデータから母分散と母平均の区間推定を行います。\n";
	cout << "信頼区間を 90%, 95%, 98%から選び数字のみ入力してください。\n";
	int confidence_interval = 0;
	do {
		cin >> confidence_interval;
		if (confidence_interval == 90 ||
			confidence_interval == 95 || 
			confidence_interval == 98 ){
			break;
		}
		cout << "信頼区間に誤りがあります。入力し直してください。\n";
	} while (true);

	//実験データの読み込み
	ifstream experiment_data_infile("experimentdata.txt");
	//実験データファイルの読み込みチェック
	if (!experiment_data_infile) {
		cout << "実験データファイルが見つかりません。\n";
		return 1;
	}
	//実験データの格納
	double temp = 0;
	vector<double> experiment_data;
	while (experiment_data_infile >> temp) {
		experiment_data.push_back(temp);
	}
	experiment_data_infile.close();

	//オブジェクトの構築
	PopulationMeanVariance common;
	PopulationMeanVariance mean;
	PopulationMeanVariance variance;
	//計算
	std_results_calculation(experiment_data, &common);
	confidence_interval_calculation(experiment_data, confidence_interval, &common, &mean, &variance);

    //計算結果の表示
	common.show_std_results();
	cout << "信頼度：" << confidence_interval << "%\n";
	cout << "母平均の";
	mean.show_confidence_interval();
	cout << "母分散の";
	variance.show_confidence_interval();
	return 0;
}