#!/sbin/busybox sh

cat << CTAG
{
    name:IO,
    elements:[
    	{ SPane:{
		title:"I/O schedulers",
		description:"Set the active I/O elevator algorithm. The scheduler decides how to handle I/O requests and how to handle them."
        }},
	{ SOptionList:{
		title:"Internal storage scheduler",
		default:`echo $(/res/synapse/actions/bracket-option /sys/block/mmcblk0/queue/scheduler)`,
		action:"bracket-option /sys/block/mmcblk0/queue/scheduler",
		values:[
`
			for IOSCHED in \`cat /sys/block/mmcblk0/queue/scheduler | sed -e 's/\]//;s/\[//'\`; do
			  echo "\"$IOSCHED\","
			done
`
		]
	}},
	{ SOptionList:{
		title:"SD card scheduler",
		default:`echo $(/res/synapse/actions/bracket-option /sys/block/mmcblk1/queue/scheduler)`,
		action:"bracket-option /sys/block/mmcblk1/queue/scheduler",
		values:[
`
			for IOSCHED in \`cat /sys/block/mmcblk1/queue/scheduler | sed -e 's/\]//;s/\[//'\`; do
			  echo "\"$IOSCHED\","
			done
`
		]
	}},
	{ SSeekBar:{
		title:"Internal storage read-ahead",
		description:"The read-ahead value on the internal phone memory.",
		max:2048, min:128, unit:"kB", step:128,
		default:`cat /sys/block/mmcblk0/queue/read_ahead_kb`,
                action:"generic /sys/block/mmcblk0/queue/read_ahead_kb"
	}},
	{ SSeekBar:{
		title:"SD card read-ahead",
		description:"The read-ahead value on the external SD card.",
		max:2048, min:128, unit:"kB", step:128,
		default:`cat /sys/block/mmcblk1/queue/read_ahead_kb`,
                action:"generic /sys/block/mmcblk1/queue/read_ahead_kb"
	}},
	{ SSpacer:{
		height:1
	}},
	{ SPane:{
		title:"General I/O Tunables",
		description:" Set the internal storage general tunables"
	}},
	{ SSpacer:{
		height:1
	}},
	{ SCheckBox:{
		description:" Draw entropy from spinning (rotational) storage. Default is Disabled",
		label:"Add Random",
		default:`cat /sys/block/mmcblk0/queue/add_random`,
		action:"ioset queue add_random"
	}},
	{ SSpacer:{
		height:1
	}},
	{ SCheckBox:{
		description:" Maintain I/O statistics for this storage device. Disabling will break I/O monitoring apps. Default is Enabled.",
		label:"I/O Stats",
		default:`cat /sys/block/mmcblk0/queue/iostats`,
		action:"ioset queue iostats"
	}},
	{ SSpacer:{
		height:1
	}},
	{ SCheckBox:{
		description:" Treat device as rotational storage. Default is Disabled",
		label:"Rotational",
		default:`cat /sys/block/mmcblk0/queue/rotational`,
		action:"ioset queue rotational"
	}},
	{ SSpacer:{
		height:1
	}},
	{ SSeekBar:{
		title:" No Merges",
		description:" Types of merges (prioritization) the scheduler queue for this storage device allows. Default is All.",
		default:`cat /sys/block/mmcblk0/queue/nomerges`,
		action:"ioset queue nomerges",
		values:{
			`NM='0:"All", 1:"Simple Only", 2:"None",'
			echo $NM`
		}
	}},
	{ SSpacer:{
		height:1
	}},
	{ SSeekBar:{
		title:"RQ Affinity",
		description:" Try to have scheduler requests complete on the CPU core they were made from. Higher is more aggressive. Some kernels only support 0-1. Default is 1.",
		default:`cat /sys/block/mmcblk0/queue/rq_affinity`,
		action:"ioset queue rq_affinity",
		values:{
			`RQA='0:"0: Disabled", 1:"1", 2:"2"'
			echo $RQA`
		}
	}},
	{ SSpacer:{
		height:1
	}},
	{ SPane:{
		title:"I/O Scheduler Tunables"
	}},
	{ SSpacer:{
		height:1
	}},
	{ SSeekBar:{
		title:"NR Requests",
		description:" Maximum number of read (or write) requests that can be queued to the scheduler in the block layer.",
		step:128,
		min:128,
		max:2048,
		default:`cat /sys/block/mmcblk0/queue/nr_requests`,
		action:"generic /sys/block/mmcblk0/queue/nr_requests",
	}},
	{ SSpacer:{
		height:1
	}},
	{ STreeDescriptor:{
		path:"/sys/block/mmcblk0/queue/iosched",
		generic: {
			directory: {},
			element: {
				SGeneric: { title:"@BASENAME" }
			}
		},
		exclude: [ "weights", "wr_max_time" ]
	}},
    ]
}
CTAG
