import React from 'react';
import ReactDOM from 'react-dom';
import './styles/index.css';
import App from './App';

/*eslint-disable */
new QWebChannel(qt.webChannelTransport, function (channel) {
    window.addEventListener('mouseup', function(){ //this event listener doesn't need to be removed since it's not a part of the React flow
        if(channel.objects.playerObject.isDraggingSeekbar){
            channel.objects.playerObject.playerClass.seek(document.querySelector('.progress-bar').value); //provisory, little messy solution - can be redone by a more experienced frontend programmer later
            channel.objects.playerObject.isDraggingSeekbar = false;
        }
    })

    channel.objects.appObject.appLoaded.connect(function() {
        ReactDOM.render(<App playerObject={channel.objects.playerObject} />, document.getElementById('root')); //run React
    });

    channel.objects.appObject.loadApp(); //calls the app to load
});
/*eslint-enable */