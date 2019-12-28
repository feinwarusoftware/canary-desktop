console.log(window.location.pathname);

var playerObject;

var isDragging = false;

var library, albums = {};

const Foo = { template: '<div>foo</div>' }
const Bar = { template: '<div>bar</div>' }

const router = new VueRouter({
    routes: [
      {
        path: '/album/:id',
        name: 'album',
        component: AlbumPage
      },
      { path: '/foo', component: AlbumPage },
      { path: '/bar', component: Bar }
    ]
});

var app = new Vue({
    el: '#app',
    data: {
      location:'',
      currentPos: 0,
      trackLength:0,
      cover:"",
      artist:"-",
      album:"-",
      title:"-",
      albumsLoaded:false,
      albums:{},
      playerObject:playerObject
    },
    methods:{
        toMSS:toMSS
    },
    router
});

var appInfo = {};

window.onload = function () {
    new QWebChannel(qt.webChannelTransport, function (channel) {
        playerObject = channel.objects.playerObject; //defines playerObject after loading
        app.playerObject = channel.objects.playerObject;

        playerObject.setCSInfo.connect(function(len, coverData, artist, album, title) {
            app.trackLength = len;
            app.cover = coverData;
            app.artist = artist;
            app.album = album;
            app.title = title;
        });

        playerObject.setCSPos.connect(function(pos) {
            if(!isDragging){
                app.currentPos = pos;
            }
        });

        playerObject.appLoaded.connect(function(location){
            app.location = location;
        });

        playerObject.loadApp();

        playerObject.libraryLoaded.connect(function(lib) {
            library = JSON.parse(lib);

            var loadedAlbums = [];

            for(i=0;i<library.length;i++){
                if(!loadedAlbums.includes(library[i].albumid)){ //if it's a new album
                    albums[library[i].albumid] = [];

                    app.albums[library[i].albumid] = [];

                    loadedAlbums.push(library[i].albumid);

                    /*data='${JSON.stringify(library[i])}'*/

                    //document.querySelector('.coverList').innerHTML += `<div class="albumCover" style="background-image:url('file:///${appInfo.location}/cache/covers/${library[i].albumid}.png'" data-id="${library[i].albumid}");"></div>`;
                    //<router-link :to="{name: 'album', params: {id: 1}}" class="title">Album 1</router-link>
                }

                albums[library[i].albumid].push(library[i]); //push song to it's respective album object

                app.albums[library[i].albumid].push(library[i]);
                //TODO: MINIMIZE DATA HERE
            }

            console.log(loadedAlbums);
            //console.log(albums);
            console.log(app.albums)
            app.albumsLoaded = true;
        });

        playerObject.getLibrary();
    });
}

document.querySelector('.volumeSlider').addEventListener('input', (e) => {
    document.querySelector('.debug').innerHTML = "Volume: " + e.target.value;

    playerObject.changeVolume(e.target.value);
});

document.querySelector('.progressBar').addEventListener('input', (e) => {
    isDragging = true;

    //TODO: replace for VUE SYNC (so currenPos changes on the fly, thus changing the cspos value)
    app.currentPos = e.target.value;

    console.log(app.currentPos);
});

document.querySelector('.progressBar').addEventListener('change', (e) => {
    isDragging = false;

    playerObject.seek(e.target.value);

    console.log('input')
});

function toMSS(seconds){
    let minutes = Math.floor(seconds / 60);
    let remainingSeconds = seconds - 60 * minutes;

    if(remainingSeconds <= 9){
        remainingSeconds = "0" + remainingSeconds;
    }

    return minutes + ":" + remainingSeconds;
}

function oi(){
    console.log('oi')
}