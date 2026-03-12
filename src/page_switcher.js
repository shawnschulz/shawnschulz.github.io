function showPage(id) {
  document.querySelectorAll('.page').forEach(p => p.classList.remove('active'));
  document.getElementById('page-' + id).classList.add('active');
  history.pushState({}, '', '#' + id);
}

// Handle back/forward
window.addEventListener('popstate', () => {
  const id = location.hash.replace('#', '') || 'about';
  showPage(id);
});

// Load correct page on initial visit
const id = location.hash.replace('#', '') || 'about';
showPage(id);
