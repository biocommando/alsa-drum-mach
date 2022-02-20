#! /usr/bin/python3
import os
import re
import sys

dirname = './'
if len(sys.argv) > 1:
    dirname = sys.argv[-1]
if not dirname.endswith('/'):
    dirname = dirname + '/'

files = os.listdir(dirname)
files = list(filter(lambda x: x.endswith('.wav') or x.endswith('.WAV'), files))
files = sorted(files)

conf = '''. Automatically generated drum machine config file
I common_conf.vars 0

. Ranges
{set_var} speed_min   0.5
{set_var} speed_max   1.5
{set_var} volume_min  0
{set_var} volume_max  2
{set_var} fpreovd_min 0
{set_var} fpreovd_max 4

'''

conf = conf + f'{{slots}}  {len(files)}\n'

slot_id = 0
cc_num = 0
slot_names = []

for file in files:
    conf = conf + '\n'
    slot_name = re.sub(' ', '_', file)
    if slot_name != file:
        os.rename(dirname + file, dirname + slot_name)
        file = slot_name
    slot_name = re.sub('.wav', '', slot_name)
    slot_name = re.sub('.WAV', '', slot_name)
    if len(slot_name) > 15:
        slot_name = slot_name[:15]
    i = 0
    orig_slot_name = slot_name
    while slot_name in slot_names:
    	slot_name = (str(i) + orig_slot_name)[:15]
    	i = i + 1
    slot_names.append(slot_name)
    conf = conf + f'{{set_var}}   {slot_name}       {slot_id}\n'
    conf = conf + f'{{load}}      {{{slot_name}}}     {file}\n'
    conf = conf + f'{{cc}}        {{{slot_name}}}     {cc_num}                 {{speed_min}} {{speed_max}}\n'
    cc_num = cc_num + 1
    conf = conf + f'{{cc}}        {{{slot_name}}}     {cc_num}                 {{volume_min}} {{volume_max}}\n'
    cc_num = cc_num + 1
    slot_id = slot_id + 1

conf = conf + '''
. Global filter settings
{set_param} {global}    {filter_enable} {enabled}
'''
conf = conf + '{cc}        {global}    {filter_cutoff}  ' + str(cc_num)
conf = conf + '\n{cc}        {global}    {filter_resonc}  ' + str(cc_num + 1)
conf = conf + '\n{cc}        {global}    {filter_preovd}  ' + str(cc_num + 2) + ' {fpreovd_min} {fpreovd_max}'

conf = conf + '\n\n{end_conf}\n'

with open(dirname + 'sample_data_config.txt', 'w') as f:
    f.write(conf)

