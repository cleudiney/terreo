const CACHE_NAME = 'caixa-cache-v4';

const urlsToCache = [
  '/',
  '/index.html',
  '/login-localstorage.html',
  '/adm.html',
  '/manifest.webmanifest',
  '/icons/icon-192.png',
  '/icons/icon-512.png',
  '/css/base.css',
  '/css/navbar.css',
  '/css/login-localstorage.css',
  '/css/admin.css',
  '/js/navbar.js',
  '/js/auth-guard.js',
  '/js/session.js',
  '/js/login-localstorage.js',
  '/js/admin.js',
  '/js/pwa-register.js',
  '/partials/navbar.html'
];

self.addEventListener('install', (event) => {
  self.skipWaiting();
  event.waitUntil((async () => {
    const cache = await caches.open(CACHE_NAME);
    await Promise.all(urlsToCache.map(async (url) => {
      try {
        const resp = await fetch(url, { cache: 'no-cache' });
        if (resp.ok) await cache.put(url, resp.clone());
      } catch (_) {}
    }));
  })());
});

self.addEventListener('activate', (event) => {
  event.waitUntil((async () => {
    const keys = await caches.keys();
    await Promise.all(keys.map((k) => (k !== CACHE_NAME ? caches.delete(k) : Promise.resolve())));
    await self.clients.claim();
  })());
});

self.addEventListener('fetch', (event) => {
  if (event.request.method !== 'GET') return;
  event.respondWith((async () => {
    const cached = await caches.match(event.request);
    if (cached) return cached;
    return fetch(event.request);
  })());
});
