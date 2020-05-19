let myButton = document.getElementById("button3");
myButton.onclick = function(){
    let thisText = document.querySelector('#button3').innerHTML;
    if(thisText=="Forward Crawl"){
        document.querySelector('#button3').innerHTML = "Crawling";
    }
    else{
        document.querySelector('#button3').innerHTML = " ";
    }
}
