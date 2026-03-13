function showPage(id) {
  document.querySelectorAll('.page').forEach(p => p.classList.remove('active'));
  document.getElementById('page-' + id).classList.add('active');
  history.pushState({}, '', '#' + id);
}

// Uses the gridCell object to generate a page with grids
function generateGridPage(gridCell) {
}

function generate_blog_post(post_name) {
  var blogPage = document.getElementById("page-blog");
  var text_element = document.createElement("p")
  let fr = new FileReader();
  let post_body =  fr.readAsText(post_name);
  text_element.appendChild(post_body);
  blogPage.appendChild(text_element);
  const id = location.hash.replace('#', '') || "blog";
  showPage(id)
}

function generate_post1() {
  generate_blog_post("./docs/assets/blog_posts/post1.txt")
}

function generate_links() {
}

var prev_page_name = 'about'

// Handle back/forward
window.addEventListener('popstate', () => {
  const id = location.hash.replace('#', '') || prev_page_name;
  showPage(id);
});

// Load correct page on initial visit
const id = location.hash.replace('#', '') || 'about';
showPage(id);
