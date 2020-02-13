import React from 'react';
import ReactDOM from 'react-dom';
import './styles/index.css';
import App from './App';
import {toMSS} from './components/UsefulFunctions';

/*eslint-disable */
new QWebChannel(qt.webChannelTransport, function (channel) {
    channel.objects.playerObject.setNowPlayingPos.connect(function(pos) {
        document.querySelector(".currentTime").innerHTML = toMSS(pos);
        document.querySelector(".progress-bar").value = pos;
      });

    channel.objects.appObject.appLoaded.connect(function() {
        ReactDOM.render(<App playerObject={channel.objects.playerObject} />, document.getElementById('root')); //run React
    });

    channel.objects.appObject.loadApp(); //calls the app to load
});
/*eslint-enable */