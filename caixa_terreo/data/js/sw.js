//sw.js
const CACHE_NAME = 'caixa-cache-v3';

const urlsToCache = [
  '/',
  '/index.html',
  '/login-localstorage.html',
  '/adm.html',

  // CSS
  '/css/base.css',
  '/css/navbar.css',
  '/css/login.css',
  '/css/admin.css',

  // JS
  '/js/navbar.js',
  '/js/auth-guard.js',
  '/js/session.js',
  '/js/login-localstorage.js',
  '/js/admin.js',

  // Partials
  '/partials/navbar.html'
];

self.addEventListener('install', event => {
  self.skipWaiting();
  event.waitUntil(
    caches.open(CACHE_NAME).then(cache => cache.addAll(urlsToCache))
  );
});

self.addEventListener('activate', event => {
  event.waitUntil(
    caches.keys().then(keys =>
      Promise.all(
        keys.map(k => k !== CACHE_NAME && caches.delete(k))
      )
    )
  );
});

self.addEventListener('fetch', event => {
  event.respondWith(
    caches.match(event.request).then(res => res || fetch(event.request))
  );
});
