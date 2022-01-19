---
title: Video Player
...

<div id="playhere">Missing video name</div>

Playback speed: <input type="text" id="speed" value="1.0" oninput="respeed()"/>

Download <a href="" id="download"></a> or <a href="" id="download2"></a>

<script type="text/javascript">
function loadVid() {
    var vid = location.hash.replace('#','lectures/')
    var vtt = vid.replace(/[.][^.]*$/,'.vtt')
    if (vid) {
        document.getElementById('playhere').innerHTML = `
<video controls repload="metadata" style="max-width:100%">
<source src="${vid}" type="video/webm">
<track label="English (AI generated)" src="${vtt}" kind="subtitles" srclang="en">
</video>
`;
        document.getElementById('download').innerHTML = vid.replace(/.*\//g, '')
        document.getElementById('download').href = vid
        document.getElementById('download2').innerHTML = vtt.replace(/.*\//g, '')
        document.getElementById('download2').href = vtt
    }
}
loadVid();

function respeed() {
    let vid = document.querySelector('video')
    if (vid) vid.playbackRate = document.querySelector('#speed').value
}
</script>
