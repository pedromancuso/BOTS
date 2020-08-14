var ID = function(id){return document.getElementById(id);};
document.addEventListener('click',function(e){
	switch (e.target && e.target.id) {
		case ('menu'):ID('nav').style.display='block';break;
		case ('main'):ID('nav').style.display='none';break;
		case ('closenav'):ID('nav').style.display='none';break;
		case ('passwordbutton'):ID('passwordsurvey').style.display='inline-block';break;
		case ('cancel'):ID('passwordsurvey').style.display='none';break;
		case ('changepass'):passwordxml();break;
		case ('dispositivostab'):dispositivos(1);break;
		case ('programastab'):dispositivos(0);break;
		case ('agregarprograma'):programeditshow(0);break;
		case ('canceledit'):programeditshow(0);break;
		case ('addprogram'):programeditshow(0);break;
		case ('eliminarprogramas'):eliminarprogramas();break;
		case ('saveedit'):nuevoprograma();break;
		case ('trash'):delmenu();break;
		case ('connect'):wificonfigxml();break;
		case ('login'):botsloginxml();break;
		case ('recuperarcontraseña'):recutoggle();break;
		case ('cancelrecu'):recutoggle();break;
		case ('recuperar'):recuperarxml();break;
		default:break;
	}
	for (var i = 0; i <= max; i++) {
		if(e.target && e.target.id== ('nombreprograma'+i)){programeditshow(i);}
		if(e.target && e.target.id== ('check'+i)){check(i);}
		if(e.target && e.target.id== ('programaestado'+i)){modificarestadonprograma(i);}
		}
	for (var i = 1; i <= 7; i++) {if(e.target && e.target.id== ('D'+i)){selectday('D'+i);}}
});
function myajax(stringpost,url){
	return new Promise(resolve => {
		setTimeout(() => {
			var xmlHttp = new XMLHttpRequest();
			xmlHttp.onreadystatechange=function() {
				if (xmlHttp.readyState === 4 && xmlHttp.status === 200) {
					resolve(xmlHttp.responseXML.getElementById('respxml').childNodes[0].nodeValue);
				}
			};
			xmlHttp.open('POST',url,true);
			xmlHttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
			xmlHttp.send(stringpost);
		}, 2000);
	});
}
var max=0;var res=[];
async function mqtt(){
	res=[];
	var message = await myajax(null,'/htmlclientesxml');
	ID('clientes').innerHTML='';
	var progx = message.split('$');
	if (progx.length>1) {
		for (var i = 0; i < progx.length-1; i++) {
			res[i] = progx[i].split('?');if (res[i][0]>max) {max=res[i][0];}
			ID('clientes').innerHTML+=
			`<tr><td class='option'>`+res[i][1]+`</td><td style='text-align: right;'><label id='label`+res[i][0]+`' class='switch'><input id='check`+res[i][0]+`' type='checkbox' class='optioninput'><span class='slider round'></span></label><span id='estado`+res[i][0]+`' class='estado'></span></td></tr>`;
	}
	}else{ID('programasclientes').innerHTML+=`<tr style='cursor:pointer'><td id='nodev' class='mytd'>NO DEVS</td></tr>`;}
	programasclientes();
}
var programasres=[];
async function programasclientes(){	
	programasres=[];
	var message=await myajax(null,'/htmlprogramasclientesxml');
	ID('programasclientes').innerHTML='';
	ID('devicesprogram').innerHTML='';
	var progx=message.split('$');
	if (progx.length>1){
		for (var i=0;i<progx.length-1;i++) {
			programasres[i]=progx[i].split('?');
			if (programasres[i][0]>max) {max=programasres[i][0];}
			ID('programasclientes').innerHTML+=
			`<tr id='programa`+programasres[i][0]+`' style='cursor:pointer;'><td class='deleteop mytd' id='deleteprogram`+programasres[i][0]+`' style='display: none;'><input type='checkbox' id='del`+programasres[i][0]+`' checked data-toggle='toggle'><label for='del`+programasres[i][0]+`'></label></td><td class='mytd' id='nombreprograma`+programasres[i][0]+`' style='width: 100%;'>`+programasres[i][1]+`</td><td class='mytd' style='text-align: right;width: 25%'><label class='switch' style='display:block'><input type='checkbox' id='programaestado`+programasres[i][0]+`' class='optioninput'><span class='slider round'></span></label></td></tr>`;					
		}
		for (var i=0;i<programasres.length-1;i++){
			var u= ID('programaestado'+programasres[i][0]);///aca esta el problema
			if (programasres[i][2]==1){u.checked=true;}else if(programasres[i][2]==0){u.checked=false;}
		}
		
	}else{
		ID('programasclientes').innerHTML+=`<tr style='cursor:pointer'><td id='agregarprograma' class='mytd'>+ Agregar programa</td></tr>`;
	}
	for (var j=0;j<res.length;j++){
		ID('devicesprogram').innerHTML+=`<tr><td class='mytd'>`+res[j][1]+`</td><td class='mytd' style='text-align: right;'><input type='checkbox' id='devicesprogram`+res[j][0]+`'><label for='devicesprogram`+res[j][0]+`'></label></td></tr>`;
	}
	setTimeout(receive,1000);
}
function recutoggle(){
	var x1 = ID('recucontramod');var x2 = ID('botsloginform');
	if (x1.style.display=='block') {x1.style.display='none';x2.style.display='block';}else {x1.style.display='block';x2.style.display='none';}
}
function selectday(a){
	ID(a).value=!ID(a).value;
	if (ID(a).value==false){ID(a).className = 'day special disabled';}
	if (ID(a).value==true){ID(a).className = 'day special';}
}
function delmenu() {
	var cols = document.getElementsByClassName('deleteop');
	for(i = 0; i < cols.length; i++) {
		if (cols[i].style.display=='none'){cols[i].style.display='inline-block';ID('eliminarprogramas').style.display='table-cell';}
		else{cols[i].style.display='none';ID('eliminarprogramas').style.display='none';}
	}
	j1();
}
function j1(){
var x1=document.getElementById('1');var x3=document.getElementById('3');
var x2=document.getElementById('2');var x4=document.getElementById('4');
if (x1.style.display=='none') {x2.style.display='inline-block';x1.style.display='none';x3.style.display='none';x4.style.display='inline-block';}
else{x2.style.display='none';x1.style.display='inline-block';x3.style.display='inline-block';x4.style.display='none';}
}

function dispositivos(s){
	ID('PROGRAMASCONTAINER').className = 'container';
	ID('programas').style.display='inline-block';
	ID('modificacionprograma').style.display = 'none';
	ID('eliminarprogramas').style.display='none';
	ID('main').className = 'wrapper';
	if(s==1){
		ID('dispositivostab').className = 'inn selected';
		ID('programastab').className = 'inn';
		ID('CLIENTESCONTAINER').style.display = 'block';
		ID('PROGRAMASCONTAINER').style.display = 'none';
	}else if (s==0) {
		ID('dispositivostab').className = 'inn';
		ID('programastab').className = 'inn selected';
		ID('CLIENTESCONTAINER').style.display = 'none';
		ID('PROGRAMASCONTAINER').style.display = 'block';
		var cols = document.getElementsByClassName('deleteop');for(i = 0; i < cols.length; i++) {cols[i].style.display='none';}
		for (var i = 0; i <= programasres.length-1; i++) {
			var u= ID('programaestado'+programasres[i][0]);
			if (programasres[i][2]==1) {u.checked=true;} else if(programasres[i][2]==0) {u.checked=false;}
		}
	}
}
function programeditshow(IDx){
	if (IDx==0) {
		ID('programid').value='0';
		ID('editarprogramatitulo').innerHTML='NUEVO PROGRAMA';
		ID('nombreprogramaedit').value='';
		for (var i = 1; i <= 7; i++) {ID('D'+i).value=false;ID('D'+i).className='day special disabled';}
		ID('HORAINICIO').value='HH';
		ID('HORAFIN').value='HH';
		for (var i = 0; i < res.length; i++) {ID('devicesprogram'+res[i][0]).checked=false;}
	}else{
		ID('editarprogramatitulo').innerHTML='MODIFICAR PROGRAMA';
		for (var i = 0; i < programasres.length; i++) {
			if(programasres[i][0]==ID){
				ID('programid').value=programasres[i][0];
				ID('nombreprogramaedit').value=programasres[i][1];
				var days = programasres[i][3].split('D');
				for (var d = 0; d <= days.length-1; d++) {if(days[d]!='D' && days[d]!=''){selectday('D'+days[d]);}}
				var inicio = programasres[i][4].split(':');
				ID('HORAINICIO').value=inicio[0];
				var fin = programasres[i][5].split(':');
				ID('HORAFIN').value=fin[0];
				var dev =programasres[i][6].split('%');
				for (var v = 0; v < dev.length; v++) {if(dev[v]!='%' && dev[v]!=''){ID('devicesprogram'+dev[v]).checked=true;}}
			}
		}
	}
	if(ID('programas').style.display==='none'){ID('programas').style.display='inline-block';ID('modificacionprograma').style.display='none';ID('main').className ='wrapper';}
	else{ID('programas').style.display='none';ID('modificacionprograma').style.display='inline-block';ID('main').className='wrapper border';}
	ID('tabladevices').style.height=(((ID('programbutton').getBoundingClientRect().top)-(ID('titulotabla').getBoundingClientRect().bottom)))+'px';
}
function check(idx){if (ID('check'+idx).checked===true){send(idx+'I');}else{send(idx+'O');}}
async function wificonfigxml() {
	var msg = await myajax('STAssid:'+ID('STAssid').value+'?STApassword:'+ID('STApassword').value+'?','/wificonfigxml');
	if (msg=='1') {window.location.href = '/botslogin';}
	else{ID('result').innerHTML = msg;}
}
async function botsloginxml(){
	ID('result').innerHTML = 'Ingresando...';
	var msg = await myajax('BOTSUSER:'+ID('BOTSUSER').value+'?BOTSPASS:'+ID('BOTSPASS').value+'?','/botsloginxml');
	if (msg=='1'){window.location.href = '/user';}else{ID('result').innerHTML = msg;}
}
async function recuperarxml(){
	if(ID('mailrecu').value!=''){
		if(ID('recuuser').value!=''){
			ID('resultrecu').innerHTML = await myajax('user:'+ID('recuuser').value+'?email:'+ID('mailrecu').value+'?','/recupassxml');
		}else{ID('resultrecu').innerHTML='Complete Usuario';}
	}else{ID('resultrecu').innerHTML='Complete E-mail';}
}
async function passwordxml(){
	var message = await myajax('oldpassword:'+ID('oldpassword').value+'?newpassword1:'+ID('newpassword1').value+'?newpassword2:'+ID('newpassword2').value+'?','/passwordxml');
	if (message==1) {ID('result').innerHTML='Contraseña Modificada Correctamente.';}else{ID('result').innerHTML=message;}

}
async function receive(){
	var message = await myajax(null,'/htmlreceivexml');
	var estado = message.substring(message.length - 1, message.length);
	var id = message.substring(0, message.length-1);
	ID('estado'+id).innerHTML=estado;
	if(estado=='I'){ID('check'+id).checked=true;setTimeout(receive,500);}
	if(estado=='O'){ID('check'+id).checked=false;setTimeout(receive,500);}
}	

async function eliminarprogramas(){
	var progamaeliminar='?';
	for (var i=0;i<programasres.length;i++) {
		if(ID('del'+programasres[i][0]).checked==true){progamaeliminar+=programasres[i][0]+'?';}
	}
	var message = await myajax({progamaeliminar:progamaeliminar},'/htmlprogamaeliminarxml');
	programasclientes();
	delmenu();
}
async function modificarestadonprograma(u){await myajax({ID:u,ESTADO:ID('programaestado'+u).checked},'/htmlmodificarestadoprogramaxml');programasclientes();}
async function send(dato){await myajax('strtopic:'+dato+'?','/htmlsendxml');}
async function nuevoprograma(){
	var programid=ID('programid').value;
	var nombreprograma=ID('nombreprogramaedit').value;
	if (nombreprograma!='') {
		var days='';
		for (var i=1;i<=7;i++) {if(ID('D'+i).value==true){days+='D'+i;}}
		if (days!='') {
			var horainicio = ID('HORAINICIO').value+':'+ID('MINUTOINICIO').value;
			if (ID('HORAINICIO').value!='HH' && ID('MINUTOINICIO').value!='MM') {
				var horafin = ID('HORAFIN').value+':'+ID('MINUTOFIN').value;
				if (ID('HORAFIN').value!='HH' && ID('MINUTOFIN').value!='MM') {
					var devi = '%';
					for (var i=0;i<res.length;i++){if(ID('devicesprogram'+res[i][0]).checked==true){devi+=res[i][0]+'%';}}
					if (devi!='%') {
						var sta =true;
						if (programid!=0) {sta=ID('programaestado'+programid).checked;}
						await myajax({estado:sta,programid:programid,nombreprograma:nombreprograma,days:days,horainicio:horainicio,horafin:horafin,devi:devi},'/htmlnuevoprogramaxml');
						programasclientes();
						programeditshow(0);
					}else{alert('DISPOSITIVOS NO SELECCIONADOS');}
				}else{alert('HORA FIN INCORRECTA');}
			}else{alert('HORA INICIO INCORRECTA');}
		}else{alert('SIN DIAS SELECCIONADOS');}
	}else{alert('INGRESE NOMBRE DE PROGRAMA');}
}
function user(){
	var HeaderHeight=ID('header').getBoundingClientRect().height;
	var footerHeight=ID('footeruser').getBoundingClientRect().height;
	var mainHeight=document.body.clientHeight-HeaderHeight-footerHeight;
	ID('main').style.top=HeaderHeight+'px';ID('main').style.bottom=footerHeight+'px';
	/*mqtt();*/
}
if(document.body.contains(ID('webuser'))){console.log('webuser');user();}
if(document.body.contains(ID('weblogin'))){console.log('weblogin');}
if(document.body.contains(ID('weblanding'))){console.log('weblanding');}