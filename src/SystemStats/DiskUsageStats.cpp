/*
 * DiskUsageStats.cpp
 *
 *  Created on: 12-09-2009
 *      Author: paweltryfon
 */

 #include <SystemStats/DiskUsageStats.h>
 #include <Errors/ErrorConsole.h>
 
using namespace SystemStatsLib;

DiskUsageStats::DiskUsageStats(): 
		go_page_reads_stat(ACCESSTYPE_SIZE), 
		op_page_reads_stat(ACCESSTYPE_SIZE), 
		go_page_writes_stat(ACCESSTYPE_SIZE), 
		op_page_writes_stat(ACCESSTYPE_SIZE)
{
	active_read_speed_stat = &go_read_speed_stat;
	active_write_speed_stat = &go_write_speed_stat;
	active_page_writes_stat = &go_page_writes_stat;
	active_page_reads_stat = &go_page_reads_stat;
}

void DiskUsageStats::add_read_time(int bytes, double milisec) {
  	debug_print(Errors::ErrorConsole::get_instance(Errors::EC_STATS), "DiskUsageStats::add_read_time\n");
	if (milisec > 0 && bytes > 0) {
		double speed = (1.0 * bytes * 1024) / (milisec * 1000);
		active_read_speed_stat->add(speed);
	}
}

double DiskUsageStats::get_min_read_speed() const {
	return active_read_speed_stat->get_min();
}

double DiskUsageStats::get_max_read_speed() const {
	return active_read_speed_stat->get_max();
}

double DiskUsageStats::get_avg_read_speed() const {
	return active_read_speed_stat->get_avg();
}

void DiskUsageStats::add_write_time(int bytes, double milisec) {
	if (milisec > 0 && bytes > 0) {
		double speed = (1.0 * bytes * 1024) / (milisec * 1000);
		active_write_speed_stat->add(speed);
	}

}

double DiskUsageStats::get_min_write_speed() const {
	return active_write_speed_stat->get_min();
}

double DiskUsageStats::get_max_write_speed() const {
	return active_write_speed_stat->get_max();
}

double DiskUsageStats::get_avg_write_speed() const {
	return active_write_speed_stat->get_avg();
}

StatsOutput * 
DiskUsageStats::get_stats_output(std::string s) const
{
	StatsOutput* result = new StatsOutput();
	this->get_stats_output(result, s);
	return result;
}

void 
DiskUsageStats::get_stats_output(StatsOutput * result, std::string s) const
{
	result->int_names.push_back(s + "_disk_page_reads");
	result->int_stats.push_back(get_disk_page_reads());
	result->int_names.push_back(s + "_page_reads");
	result->int_stats.push_back(get_page_reads());
	result->double_names.push_back(s + "_page_reads_hit");
	result->double_stats.push_back(get_page_reads_hit());
	result->int_names.push_back(s + "_disk_page_writes");
	result->int_stats.push_back(get_disk_page_writes());
	result->int_names.push_back(s + "_page_writes");
	result->int_stats.push_back(get_page_writes());
	result->double_names.push_back(s + "_page_writes_hit");
	result->double_stats.push_back(get_page_writes_hit());
	result->double_names.push_back(s + "_avg_read_speed");
	result->double_stats.push_back(get_avg_read_speed());
	result->double_names.push_back(s + "_avg_write_speed");
	result->double_stats.push_back(get_avg_write_speed());
}

DiskUsageStats::~DiskUsageStats() {
}

EmptyDiskUsageStats::~EmptyDiskUsageStats(){}
AbstractDiskUsageStats::~AbstractDiskUsageStats(){}
