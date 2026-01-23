//sw.js
const CACHE_NAME = 'caixa-cache-v2';

const urlsToCache = [
  '/',                       // raiz
  '/index.html',
  '/login.html',
  '/login-localstorage.html',
  '/adm.html',

  // CSS
  '/css/base.css',
  '/css/navbar.css',
  '/css/login.css',
  '/css/admin.css',

  // JS
  '/js/navbar.js',
  '/js/auth.js',
  '/js/api.js',
  '/js/login-ls.js',
  '/js/admin.js',

  // Partials
  '/partials/navbar.html'
];

self.addEventListener('install', event => {
  event.waitUntil(
    caches.open(CACHE_NAME).then(cache => cache.addAll(urlsToCache))
  );
});

self.addEventListener('fetch', event => {
  event.respondWith(
    caches.match(event.request).then(response => response || fetch(event.request))
  );
});
