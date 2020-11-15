var path = window.location.pathname;
var filename = path.substring(path.lastIndexOf('/')+1);
var page= filename.substring(3,filename.indexOf('.'));
var disp= filename.substring(0,3);
var botsuser;
var dns='';
var client;
var clientId;
var connect = false;
console.log('page='+page+' / disp='+disp);
var ID=function(id){return document.getElementById(id);};
var di=function(id){return ID(id).style.display;};
var val=function(id){return ID(id).value;};
var tg=function(id,sta){return ID(id).style.display=sta;};
var iH=function(id,con){return document.getElementById(id).innerHTML=con;};
var cN=function(id,con){return document.getElementById(id).className=con;};	
var app={
	initialize:function(){
		if(disp=='app'){document.addEventListener('deviceready', this.onDeviceReady.bind(this),false);}
		else{document.addEventListener('deviceready', this.onDeviceReady());}
	},
	onDeviceReady:function(){this.receivedEvent('deviceready');},
	receivedEvent:function(id){
		if (page=='landing') {
			if (disp=='app') {
				dns='https://bots.net.ar';
				console.log('window.localStorage.getItem(userclient): '+ window.localStorage.getItem('userclient'));
				if (window.localStorage.getItem('userclient')){window.location.href='./appuser.html';}
				else{tg('listening','none');tg('received','block');tog('cfgBOTS1');}
			}else if(disp=='esp'){landing();}
		}else if(page=='user'){
			if (disp=='app'){
				console.log('window.localStorage.getItem(userclient): '+ window.localStorage.getItem('userclient'));
				dns='https://bots.net.ar';
				botsuser=window.localStorage.getItem('userclient');
			}
			user();
		}
	}
};
app.initialize();
/*-------------------------------------MAPEO----------------------------------------------------------------------------------*/
document.addEventListener('click',function(e){
	switch (e.target&&e.target.id){
		case ('menu'):tog('botsnav');break;
		case ('main'):if(di('botsnav')!='none'){tog('botsnav');};break;
		case ('closenav'):tog('botsnav');break;
		case ('WiFi'):login('WiFi');break;
		case ('BOTS'):login('BOTS');break;
		case ('cancel'):tog('pass1');break;
		case ('pass'):tog('pass1');break;
		case ('devtab'):devdisplay(1);break;
		case ('prgtab'):devdisplay(0);break;
		case ('addprg'):epd(0);break;
		case ('addprg2'):epd(0);break;
		case ('canceledit'):epd(0);break;
		case ('addprg'):epd(0);break;
		case ('cfgBOTS'):tog('cfgBOTS1');break;
		case ('cfgWiFi'):tog('cfgWiFi1');break;
		case ('trash'):delmenudisplay();break;
		case ('recoverpass'):tog('recoverpass1');tog('cfgBOTS1');break;
		case ('cancelrecu'):tog('recoverpass1');tog('cfgBOTS1');break;
		case ('changepass'):changepassphp();break;
		case ('delprg'):delprgphp();break;
		case ('trashbig'):delprgphp();break;
		case ('del'):delprgphp();break;
		case ('saveedit'):newprgphp();break;
		case ('recuperar'):recuperarphp();break;
		case ('end'):botsend();break;
		default:break;
	}
		for (var y=0;y<=max;y++){
		if(e.target && e.target.id==('prg'+y)) {epd(i);}
		if(e.target && e.target.id==('nprg'+y)) {epd(y);}
		if(e.target && e.target.id==('nprgright'+y)) {epd(y);}
		if(e.target && e.target.id==('check'+y)) {check(y);}
		if(e.target && e.target.id==('prgstate'+y)) {editstateprgphp(y);}
	}
	for (var y=1;y<=7;y++){if(e.target&&e.target.id==('D'+y)){diatog('D'+y);}}
});
function myajax(stringpost,endpoint){
	return new Promise(resolve=>{
		setTimeout(()=>{
			var xmlHttp=new XMLHttpRequest();
			xmlHttp.onreadystatechange=function(){
				if(xmlHttp.readyState===4&&xmlHttp.status===200){
				try{resolve(xmlHttp.response);}
				catch(err){console.log(err);alert('resp1: '+err);}
				}
			};
			xmlHttp.open('POST',dns+endpoint,true);
			xmlHttp.setRequestHeader('Content-type','application/x-www-form-urlencoded');
			xmlHttp.send(stringpost);
		}, 2000);
	});
}
function user(){
	var HeaderHeight=ID('header').getBoundingClientRect().height;
	var footerHeight=ID('footeruser').getBoundingClientRect().height;
	var mainHeight=document.body.clientHeight-HeaderHeight-footerHeight;
	ID('main').style.top=HeaderHeight+'px';ID('main').style.bottom=footerHeight+'px';
	setbotsuser();
}
async function setbotsuser(){
	if (disp!='app') {botsuser=await myajax(null,'/retrieveuser.php');}
	if (botsuser!=''){cliphp();}else{setTimeout(setbotsuser,1000);}	
}
async function landing(){
	var msg=await myajax(null,'/LandingCheck');
	var est=msg.split('&');
	if(est[0]!='OFFLINE'){
		iH('cfgWiFi',`CONFIGURACION WIFI <span style='color: green;'>✔</span>`);
		iH('WiFires','WiFi conectado a Red: '+ est[0]);
		tog('cfgWiFi1');
		tog('cfgBOTS1');
	}
	if(est[1]!='false'){
		iH('botsres','BOTS USER: '+ est[1]);
		iH('cfgBOTS',`INICIO DE SESION BOTS <span style='color: green;'>✔</span>`);
	}
}
function prgping(){if(disp!='esp'){sendmqtt('0','ProgramsChange');}}
/*-------------------------------------HTTP----------------------------------------------------------------------------------*/
async function login(NAME){
	iH(NAME+'res','Ingresando...');
	var msg=await myajax('&user='+ID(NAME+'USER').value+'&pass='+ID(NAME+'PASS').value+'','/'+NAME+'login.php');
	if(msg=='1'){
		iH('cfg'+NAME,'INICIO DE SESION '+BOTS+`<span style='color: green;'>✔</span>`);
		tog('cfg'+NAME+'1');
		if(dns!=''){window.localStorage.setItem('userclient', ID(NAME+'USER').value);
			window.location.href='./appuser.html';}else{window.location.href='/';}
	}
	else{iH(NAME+'res',msg);iH('cfg'+NAME,'INICIO DE SESION '+NAME+' &#8964;');}
}
async function botsend(){await myajax(null,'/botsend.php');if (disp=='app') {window.localStorage.clear();}window.location.href=disp+'landing.html';}
/*-------------------------------------PHP----------------------------------------------------------------------------------*/
var max=0;var res=[];
async function cliphp(){
	res=[];
	iH('cli','');
	var msg=await myajax('&user='+botsuser+'','/cli.php');
	res = auxnewlist('cli',msg);
	prgcliphp();
}
var prgres=[];
async function prgcliphp(){	
	prgres=[];
	var msg=await myajax('&user='+botsuser+'','/prgcli.php');
	iH('prgcli','');
	iH('devprg','');
	prgres = auxnewlist('prgcli',msg);
	auxlistdev();
	setTimeout(mqttinput,1000);
}
function auxnewlist(NAME,msg){
	var temp=[];
	var progx=msg.split('$');
	if(progx.length>1){
		for (var i=0;i<progx.length-1;i++){
			temp[i]=progx[i].split('&');
			temp[i].splice(0, 1);
			for (var k = 0;k<=temp[i].length-1; k++) {
				temp[i][k]=temp[i][k].split('=')[1];
			}
			auxmax(parseInt(temp[i][0]));
			auxnew(NAME,temp[i][0],temp[i][1]);
		}
		if(NAME=='prgcli'){auxstateprg();}
	}else{auxvacio(NAME,1);}
	return temp;
}
function auxnew(NAME,NID,NIDNAME){
	if (NAME=='prgcli'){
		ID(NAME).innerHTML+=`<tr id='prg`+NID+`'><td class='mytd' id='nprg`+NID+`'>`+NIDNAME+`</td><td id='nprgright`+NID+`' class='mytd right'><label class='switch prgswitch'><input type='checkbox' id='prgstate`+NID+`'><div class='sliderround'></div></label><label class='delop switch' style='display: none;' id='delprg`+NID+`'><input type='checkbox' id='del`+NID+`'><div class='square'></div></label></td></tr>`;
	}
	if (NAME=='cli'){
		ID(NAME).innerHTML+=`<tr><td class='mytd'>`+NIDNAME+`</td><td class='mytd right'><label class='switch' id='label`+NID+`'><input type='checkbox' id='check`+NID+`'><div class='sliderround'></div></label></td></tr>`;
	}
}
function auxvacio(show){
	if (NAME=='prgcli') {
		if (show==1) {ID(NAME).innerHTML+=`<tr id='addprg'><td id='addprg2'>+ Agregar prg</td></tr>`;}
		else{ID('addprg').parentNode.removeChild(ID('addprg'));}
	}
	if (NAME=='cli') {ID(NAME).innerHTML+=`<tr><td id='nodev'>NO DEVS</td></tr>`;}
}
function auxlistdev(){for (var j=0;j<Object.keys(res).length;j++){ID('devprg').innerHTML+=`<tr><td class='mytd'>`+res[j][1]+`</td><td class='mytd right'><label class='switch'><input type='checkbox' id='devprg`+res[j][0]+`'><div class='square'></div></label></td></tr>`;}}
function auxmax(NMAX){if(NMAX>=max) {max=NMAX;}}
function auxstateprg(){for (var i=0;i<Object.keys(prgres).length;i++){
	var u=ID('prgstate'+prgres[i][0]);if(prgres[i][2]==1 || prgres[i][2]==true || prgres[i][2]=='true'){u.checked=true;}else{u.checked=false;}}}
async function recupassphp(){
	if(val('mailrecu')!=''){
		if(val('recuuser')!=''){
			iH('resrecu',await myajax('&user='+ID('recuuser').value+'&email='+ID('mailrecu').value+'','/recupass.php'));
		}else{iH('resrecu','Complete Usuario');}
	}else{iH('resrecu','Complete E-mail');}
}
async function changepassphp(){
	var msg=await myajax('&oldpass='+ID('oldpass').value+'&newpass1='+ID('newpass1').value+'&newpass2='+ID('newpass2').value+'','/changepass.php');
	if(msg==1){iH('res','Contraseña Modificada Correctamente.');}else{iH('res',msg);}
}
async function editstateprgphp(u){
	var st;
	if (ID('prgstate'+u).checked){st=true;}else{st=false;}
	var msg = await myajax('&ID='+u+'&ESTADO='+st+'','/editstateprg.php');
	if (msg==1) {ID('prgstate'+u).checked=st;}else{ID('prgstate'+u).checked!=st;console.log('resp: '+msg);alert('resp2: '+msg);}
	prgping();
}
async function newprgphp(){
	var prgid=val('prgid');
	var nprg=val('nprgedit');
	if(nprg!=''){
		if ((nprg.split('&').length<=1)&&(nprg.split('$').length<=1)){
			var days='';
			for (var i=1;i<=7;i++){if(val('D'+i)==true){days+='D'+i;}}
			if(days!=''){
				var HHINICIO=val('HHINICIO');
				if(val('HHINICIO')!=''){
					var HHFIN=val('HHFIN');
					if(val('HHFIN')!=''){
						var dev='%';
						for (var i=0;i<res.length;i++){if(ID('devprg'+res[i][0]).checked==true){dev+=res[i][0]+'%';}}
						if(dev!='%'){
							var sta =1;
							if(prgid!=0){sta=ID('prgstate'+prgid).checked;}
							var msg=await myajax('&prgid='+prgid+'&nprg='+nprg+'&state='+sta+'&days='+days+'&HHINICIO='+HHINICIO+'&HHFIN='+HHFIN+'&dev='+dev+'','/newprg.php');
							if (msg.split('NID').length==2) {
								var NID=msg.split('NID')[1];
								var preslen = Object.keys(prgres).length;
								var newp=(NID+'&'+nprg+'&'+sta+'&'+days+'&'+HHINICIO+'&'+HHFIN+'&'+dev+'&$');
								if(preslen==0) {auxvacio('prgcli',0);}
								if (prgid==0) {
									prgres[preslen]=newp.split('&');
									auxmax(parseInt(NID));
									auxnewprg(NID,nprg);
								}else{
									for (var i=0;i<preslen;i++){
										if (prgres[i][0]==NID){
											prgres[i]=newp.split('&');
											iH('nprg'+NID,nprg);
											ID('prgstate'+NID).checked=sta;
										}
									}
								}
							}else{console.log('resp: '+msg);alert('resp3: '+msg);}
							epd(0);
							auxstateprg();
						}else{alert('DISPOSITIVOS NO SELECCIONADOS');}
					}else{alert('HH FIN INCORRECTA');}
				}else{alert('HH INICIO INCORRECTA');}
			}else{alert('SIN DIAS SELECCIONADOS');}
		}else{alert('NOMBRE DE PROGRAMA NO PUEDE CONTENTER $ o &');}
	}else{alert('INGRESE NOMBRE DE PROGRAMA');}
	prgping();
}
async function delprgphp(){
	var prgdel='$';
	for (var i=0;i<Object.keys(prgres).length;i++){
		if(ID('del'+prgres[i][0]).checked==true){prgdel+=prgres[i][0]+'$';}
	}
	var msg=await myajax('&prgdel='+prgdel+'','/prgdel.php');
	if (msg==1) {
		var len=0;
		for (var i=0;i<Object.keys(prgres).length;i++){
			if(ID('del'+prgres[i][0]).checked==true){
				var row = document.getElementById('prg'+prgres[i][0]);
				row.parentNode.removeChild(row);
				prgres.splice(i, 1);
				i--;
			}
		}
		if(Object.keys(prgres).length==0) {
			if (document.body.contains(ID('addprg'))==false){auxvacio('prgcli',1);}
		}
	}else{console.log('resp: '+msg);alert('resp4: '+msg);}
	delmenudisplay();
	prgping();
}
/*-------------------------------------CSSDISPLAY----------------------------------------------------------------------------------*/
function tog(id){
	if(di(id)=='none'){tg(id,'block');}else{tg(id,'none');}
}
function diatog(x){
	ID(x).value=!val(x);
	if(val(x)==false){cN(x,'day sp off');}
	if(val(x)==true){cN(x,'day sp');}
}

function delmenudisplay(){
	var x='delprg';
	var x1=document.getElementsByClassName('delop');
	var x2=document.getElementsByClassName('prgswitch');
	if (di(x)=='none') {
		tg(x,'table-cell');
		for(i=0;i<x1.length;i++){x1[i].style.display='inline-block';x2[i].style.display='none';}
	}else{
		tg(x,'none');
		for(i=0;i<x1.length;i++){x1[i].style.display='none';x2[i].style.display='inline-block';}
	}
}
function devdisplay(s){
	tg('prg','inline-block');
	tg('editprg','none');
	tg('delprg','none');
	cN('main','wrap');
	if(s==1){
		cN('devtab','inn selected');
		cN('prgtab','inn');
		tg('clicont','block');
		tg('prgcont','none');
	}else if(s==0){
		cN('devtab','inn');
		cN('prgtab','inn selected');
		tg('clicont','none');
		tg('prgcont','block');
		if (di('delprg')!='none') {delmenudisplay();}
		auxstateprg();
	}
}
function epd(IDx){
	if(IDx==0){
		ID('prgid').value='0';
		iH('editprgtitle','NUEVO PROGRAMA');
		ID('nprgedit').value='';
		ID('HHINICIO').value='';
		ID('HHFIN').value='';
		for (var i=1;i<=7;i++){ID('D'+i).value=false;cN('D'+i,'day sp off');}
		for (var i=0;i<res.length;i++){ID('devprg'+res[i][0]).checked=false;}
	}else{
		iH('editprgtitle','MODIFICAR PROGRAMA');
		for (var i=0;i<Object.keys(prgres).length;i++){
			if(prgres[i][0]==IDx){
				ID('prgid').value=prgres[i][0];
				ID('nprgedit').value=prgres[i][1];
				var days=prgres[i][3].split('D');
				for (var d=0;d<=days.length-1;d++){if(days[d]!='D'&&days[d]!=''){diatog('D'+days[d]);}}
				ID('HHINICIO').value=auxcortarHH(prgres[i][4]);
				ID('HHFIN').value=auxcortarHH(prgres[i][5]);
				var dev =prgres[i][6].split('%');
				for (var v=0;v<dev.length;v++){if(dev[v]!='%'&&dev[v]!=''){ID('devprg'+dev[v]).checked=true;}}
			}
		}
	}
	var x='prg';
	var y='editprg';

	if(di(x)=='none'){tg(x,'inline-block');tg(y,'none');cN('main','wrap');}
	else{tg(x,'none');tg(y,'inline-block');cN('main','wrap border');}
	ID('tabladev').style.height=(((ID('prgbutton').getBoundingClientRect().top)-(ID('titletabla').getBoundingClientRect().bottom)))+'px';
}
var auxcortarHH=function(string){var cut=[];cut=string.split(':');if (cut[0].length==1) {cut[0]='0'+cut[0];}if (cut[1].length==1) {cut[1]='0'+cut[1];}return (cut[0]+':'+cut[1]);};
/*-------------------------------------MQTT----------------------------------------------------------------------------------*/
function check(idx){if(ID('check'+idx).checked===true){sendmqtt(idx,'1');}else{sendmqtt(idx,'0');}}
var url='';var port;var userName;var password;var topic;
async function mqttinput(){
	if (disp=='esp') {
		console.log('espmqttinput');
		onMessageArrived();tg('loader','none');tg('main','block');}
	else{
		console.log('app/webmqttinput');
		var message=await myajax('&mqttuser='+botsuser+'','/mqttuser.php');
		var mqttdata=message.split('&');
		if(mqttdata.length>1){
			var mqttvar=[];
			for (var i=0;i<mqttdata.length;i++){mqttvar[i]=mqttdata[i].split('=');}
			if(disp=='app'){url+='tcp://';}
			url+=mqttvar[1][1];
			if(disp=='app'){port=parseInt(mqttvar[2][1]);}else{port = parseInt(mqttvar[3][1]);}
			userName=mqttvar[4][1];
			password=mqttvar[5][1];
			topic=mqttvar[6][1];
			setTimeout(mqttconn,1000);
		}else{setTimeout(mqttinput,6000);}
	}
}
/*-------------------------------------ESP----------------------------------------------------------------------------------*/
async function sendmqtt(idu,dato){await myajax('&topic:'+idu+'&value:'+dato,'/sendmqtt');}
var tempres;
async function onMessageArrived(a){
	var resp=await myajax(null,'/receivemqtt');
	tempresp=resp;
	if (resp!=null && resp!='' ) {
		if (resp.split('&')[0]!=null && resp.split('&')[0]!='' ) {
			if (resp.split('&')[0]!='0') {ID('check'+resp.split('&')[0]).checked=parseInt(resp.split('&')[1]);}
		}
	}
}	