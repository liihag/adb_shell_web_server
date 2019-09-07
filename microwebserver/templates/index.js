
var pollhttp;
var polltmr=null;
var pollwait=null;
var increase=0;

if (window.XMLHttpRequest) {// code for IE7+, Firefox, Chrome, Opera, Safari
    pollhttp=new XMLHttpRequest();
} else {
    pollhttp=new ActiveXObject("Microsoft.XMLHTTP");
}

//document.getElementById('hint').innerHTML = 'success';
function startapp()
{
    var data = document.getElementById('startdata');
    var errorinfo = document.getElementById('errorinfo');
    if(data.value)
    {
        var params = 'start=' + data.value;
        if (window.XMLHttpRequest) {// code for IE7+, Firefox, Chrome, Opera, Safari
          posthttp=new XMLHttpRequest();
        } else {
          posthttp=new ActiveXObject("Microsoft.XMLHTTP"); 
        }
        //posthttp.open('POST','devpost.html', true);
        posthttp.open('POST','startapp', true);
        posthttp.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
        posthttp.send(params);
        data.value = '';
        errorinfo.innerHTML = '';
    }
    else
        errorinfo.innerHTML = "please input";
}


function PollTimeout()
{
    pollhttp.abort();
    Poll();
}

function Poll()
{
    try {
        // add by zp
        var params = 'poll=discovery' + increase;
        increase += 1;
        var tmphttp=null;
        if (window.XMLHttpRequest) {// code for IE7+, Firefox, Chrome, Opera, Safari
          tmphttp=new XMLHttpRequest();
        } else {
          tmphttp=new ActiveXObject("Microsoft.XMLHTTP"); 
        }
        tmphttp.open('POST','poll', true);
        tmphttp.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
        tmphttp.send(params);



        pollhttp.open("GET", 'poll', true);
        pollhttp.onreadystatechange = PollReply;
        if (window.XMLHttpRequest) {// code for IE7+, Firefox, Chrome, Opera, Safari
            pollhttp.send(null);
        } else {// code for IE6, IE5
            pollhttp.send();
        }
        pollwait = setTimeout(PollTimeout, 3000); //3 seconds
    } catch (e) {
        pollwait = setTimeout(PollTimeout, 3000); //3 seconds
    }
}

function PollReply()
{
    var xml;
    var elem;
    if (pollhttp.readyState == 4 && pollhttp.status == 200) {
        clearTimeout(pollwait);
        xml = pollhttp.responseXML;
        elem = xml.getElementsByTagName('poll');
        //elem = xml.getElementsByTagName('body');
        if(elem.length > 0){
            var data = elem[0].getAttribute('mydata');
            //var data = elem[0].innerHTML;
            if(data)
                document.getElementById('hint').innerHTML = data;
        }
        polltmr = setTimeout(Poll, 750);
    }
}


function BED()
{
    pollhttp.abort();
    clearTimeout(polltmr);
    clearTimeout(pollwait);

    Poll();
}
