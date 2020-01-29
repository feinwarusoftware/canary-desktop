import React from 'react';
import ReactDOM from 'react-dom';
import './styles/index.css';
import App from './App';

/*eslint-disable */
new QWebChannel(qt.webChannelTransport, function (channel) {
    channel.objects.appObject.appLoaded.connect(function() {
        ReactDOM.render(<App playerObject={channel.objects.playerObject} />, document.getElementById('root')); //run React
    });

    channel.objects.appObject.loadApp(); //calls the app to load
});
/*eslint-enable */