import React from 'react';
import ReactDOM from 'react-dom';
import './styles/index.css';
import "./static/css/fa_all.scss";
import App from './App';

/*eslint-disable */
new QWebChannel(qt.webChannelTransport, function (channel) {
    channel.objects.playerObject.appLoaded.connect(function() {
        ReactDOM.render(<App playerObject={channel.objects.playerObject} location={channel.objects.playerObject.location} />, document.getElementById('root')); //run React
    });

    channel.objects.playerObject.loadApp(); //calls the app to load
});
/*eslint-enable */
