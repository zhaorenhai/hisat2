/*
 * Copyright 2018, Chanhee Park <parkchanhee@gmail.com> and Daehwan Kim <infphilo@gmail.com>
 *
 * This file is part of HISAT 2.
 *
 * HISAT 2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * HISAT 2 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with HISAT 2.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __REPEAT_BUILDER_H__
#define __REPEAT_BUILDER_H__

#include <iostream>
#include <fstream>
#include <limits>
#include <map>
#include "assert_helpers.h"
#include "word_io.h"
#include "mem_ids.h"
#include "ref_coord.h"
#include "ref_read.h"
#include "ds.h"
//#include "sstring.h"
#include "blockwise_sa.h"

using namespace std;

struct Fragments {
	bool contain(TIndexOffU pos) {
		if (pos >= start && pos < (start + length)) {
			return true;
		}
		return false;
	}

	TIndexOffU start;   // index within joined text
	TIndexOffU length;

	int frag_id;
	int seq_id;
	TIndexOffU start_in_seq;    // index within global 
	TIndexOffU start_in_block;  // index within Fasta Block
	bool first;

	string seq_name;
	string nameline;
};

struct RepeatGroup {
	string seq;
	EList<TIndexOffU> positions;
	EList<string> alt_seq;

	void merge(const RepeatGroup& rg)
	{
		alt_seq.push_back(rg.seq);

		for (int i = 0; i < rg.alt_seq.size(); i++) {
			alt_seq.push_back(rg.alt_seq[i]);
		}

		for (int i = 0; i < rg.positions.size(); i++) {
			positions.push_back(rg.positions[i]);
		}
	}

	bool empty(void) 
	{ 
		return positions.size() == 0;
	}

	void set_empty(void) 
	{ 
		positions.clear();
	}
};

// build Non-repetitive Genome
template<typename TStr>
class NRG {

public:
	NRG();
	NRG(
		EList<RefRecord>& szs,
		EList<string>& ref_names,
		TStr& s,
		string& filename,
		BlockwiseSA<TStr>& sa);

public:
	const int output_width = 60;

	EList<RefRecord>& szs_;
	EList<string>& ref_namelines_;
	EList<string> ref_names_;
	TStr& s_;
	string& filename_;


	BlockwiseSA<TStr>& bsa_;


	// mapping info from joined string to genome
	EList<Fragments> fraglist_;

	//
	EList<RepeatGroup> rpt_grp_;

	// Fragments Cache
#define CACHE_SIZE_JOINEDFRG	10
	Fragments cached_[CACHE_SIZE_JOINEDFRG];
	int num_cached_ = 0;
	int victim_ = 0;	/* round-robin */

public:

	void build(TIndexOffU rpt_len,
               TIndexOffU rpt_cnt,
               bool flagGrouping,
               TIndexOffU rpt_edit);
    
	void build_names(void);
	int map_joined_pos_to_seq(TIndexOffU joined_pos);
	int get_genome_coord(TIndexOffU joined_pos, string& chr_name, TIndexOffU& pos_in_chr);

	void build_joined_fragment(void);

	static bool repeat_group_cmp(const RepeatGroup& a, const RepeatGroup& b)
	{
		return a.positions[0] < b.positions[0];
	}

	void sort_rpt_grp(void);

	void savefile(void);
	void saveRepeatSequence(void);
	void saveRepeatGroup(void);


	void add_repeat_group(string& rpt_seq, EList<TIndexOffU>& rpt_range);
	void adjust_repeat_group(bool flagGrouping, TIndexOffU rpt_edit);
	void repeat_masking(void);

	int get_lcp(TIndexOffU a, TIndexOffU b);
};

#endif /* __REPEAT_BUILDER_H__ */