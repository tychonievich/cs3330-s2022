---
title: Spring 2022 Schedule
...

The following is the current best guess at course pacing and topics.
It may be adjusted as the semester progresses to reflect actual pacing and uptake of material.

<hr/>

<style id="schedule-css">

#schedule.calendar {
    display: grid;
    width: 100%; 
    background: rgba(0,0,0,0.125); 
    border: 0.5ex solid rgba(0,0,0,0);
    border-radius: 1.5ex; 
}
.calendar .day.Sun { grid-column: 1}
.calendar .day.Mon { grid-column: 2}
.calendar .day.Tue { grid-column: 3}
.calendar .day.Wed { grid-column: 4}
.calendar .day.Thu { grid-column: 5}
.calendar .day.Fri { grid-column: 6}
.calendar .day.Sat { grid-column: 7}

.calendar .day { 
    background: white;
    border-radius: 1ex;
    padding: .25ex .5ex;
    margin: .25ex;
    box-sizing:border-box; 
    overflow: hidden;
}


#schedule td, #schedule th { padding: 0ex; }

.calendar span.date { 
    font-size: 70.7%;
    padding-left: 0.5ex;
    float:right;
    margin-top:-0.5ex;
}
.calendar div {
    padding: 0 0.5ex 0 0.5ex;
    margin: 0 -0.5ex 0 -0.5ex;
}
.calendar div.day div:first-child {
    padding-top: 0.5ex;
    margin-top: -0.5ex;
}
.calendar div.day div:last-child {
    padding-bottom: 0.5ex;
    margin-bottom: -0.5ex;
}


.agenda { display: block; }

.agenda .day.newweek {
    border-top: thick solid grey;
    min-height: 2em;
}
.agenda .day {
    display: block; border-top: thin solid grey; width: 100%;
    padding: 0;
}
.agenda span.date.w0:before { content: "Sun "; }
.agenda span.date.w1:before { content: "Mon "; }
.agenda span.date.w2:before { content: "Tue "; }
.agenda span.date.w3:before { content: "Wed "; }
.agenda span.date.w4:before { content: "Thu "; }
.agenda span.date.w5:before { content: "Fri "; }
.agenda span.date.w6:before { content: "Sat "; }
.agenda span.date {
    font-size: 70.7%; width:7em;
    vertical-align: middle; 
    display: table-cell;
    padding: 0 0.5ex;
}
.agenda div.events { display: table-cell; vertical-align: middle; }

.task:before { content: "due: "; font-size: 70.7%; }
.lab:before { content: "lab: "; font-size: 70.7%; }
.lab summary:before { content: "lab: "; font-size: 70.7%; }
details.lab:before { content: ""; }
small { opacity: 0.5; }
.special, .task:before { background: rgba(255,127,0,0.25); opacity: 0.75; }
.day.exam { background: rgba(255,231,207,1) !important; }
span.date { font-family:monospace; }
details { padding-left: 1em; }
summary { margin-left: -1em; }

.day.past { opacity: 0.707; }
.day.today { box-shadow: 0 0 0.5ex 0.5ex grey; }
.agenda .day.today .wrapper { margin: 0.5ex 0;}

div.day.empty { background: rgba(0,0,0,0); padding: 0em; margin: 0em; border: none; border-radius: 0; }
.calendar div.day.empty { min-height: 1.5em; }
.agenda div.day.empty.Sun { border-top: thick solid gray; }

</style>


<p>View as 
<label><input type="radio" name="viewmode" onchange="viewmode(this)" value="calendar" id="viewmode=calendar"> calendar</label>
or
<label><input type="radio" name="viewmode" onchange="viewmode(this)" checked value="agenda" id="viewmode=agenda"> agenda</label>;
<label><input type="checkbox" name="showpast" onclick="showPast(this)" checked id="showpast"> show past</label>;
<!--
readings can be <input type="button" value="shown" onclick="document.querySelectorAll('details').forEach(x => x.setAttribute('open','open'))"></input> or <input type="button" value="hidden" onclick="document.querySelectorAll('details').forEach(x => x.removeAttribute('open'))"></input> as a whole, or clicked on individually to toggle visibility.
-->
readings can be <input type="button" value="shown" onclick="showReading(this)"></input>
or <input type="button" value="hidden" onclick="showReading(this)"></input>
as a whole, or clicked on individually to toggle visibility.
</p>



{#include schedule.html}


<script src="schedule.js"></script>


To subscribe to the above calendar, add to your calendar application of choice one or more of the following:

- <http://www.cs.virginia.edu/luther/3330/S2022/all.ics> -- the lecture and both lab sections
- <http://www.cs.virginia.edu/luther/3330/S2022/CS3330-200.ics> -- the lecture section
- <http://www.cs.virginia.edu/luther/3330/S2022/CS3330-201.ics> -- one lab section
- <http://www.cs.virginia.edu/luther/3330/S2022/CS3330-202.ics> -- one lab section

<hr/>

The <a href="https://registrar.virginia.edu/exam-schedule-spring-2022">final exam schedule</a> puts our final Tuesday 10 May at 2:00 pm. The final is an in-person on-paper exam administered in our lecture classroom.

