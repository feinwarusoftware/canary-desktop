import React from 'react';
import ReactDOM from 'react-dom';
import './styles/index.css';
import "./static/css/fa_all.scss";
//import './static/font/fontawesome/all.css';
import App from './App';
//import * as serviceWorker from './serviceWorker';

  /*eslint-disable */
  new QWebChannel(qt.webChannelTransport, function (channel) {
    ReactDOM.render(<App playerObject={channel.objects.playerObject} />, document.getElementById('root'));
  });
  /*eslint-enable */

// If you want your app to work offline and load faster, you can change
// unregister() to register() below. Note this comes with some pitfalls.
// Learn more about service workers: https://bit.ly/CRA-PWA
//serviceWorker.register();
