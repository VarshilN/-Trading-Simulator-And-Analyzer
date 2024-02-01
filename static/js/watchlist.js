document.addEventListener("DOMContentLoaded", function () {
  function udpate() {
    fetch('/update_watchList')
      .then(response => response.json())
      .catch(error => console.error('Error updating watchlist:', error));
  }
  setInterval(updateWatchlist, 300000);
})