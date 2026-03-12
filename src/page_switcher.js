function showPage(id) {
  console.log("got page switch")
  document.querySelectorAll('.page').forEach(p => p.classList.remove('active'));
  document.getElementById('page-' + id).classList.add('active');
}

// Show default page on load
showPage('about');
