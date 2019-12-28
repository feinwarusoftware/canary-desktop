const AlbumPage = {template: `<div>

<span v-for="track in this.$route.params.tracks" v-on:click="this.playerObject.insertToQueue(0, track.dir); this.playerObject.loadSong(0);"> {{ track.title[0] }} <br> </span>

</div>`};

//album id = > this.$route.params.id

//playerObject.insertToQueue(0, track.dir)