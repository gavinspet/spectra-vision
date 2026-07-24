# Spectra Vision - Deployment Guide

## Overview

Spectra Vision consists of two main components:
1. **Backend (C++)**: REST API server for emotion detection
2. **Frontend**: Web UI (can use Python Flask or React/Next.js)

This guide covers deployment strategies for both.

## Backend Deployment Options

### Option 1: Railway.app (Recommended for C++)

Railway supports Docker deployment, perfect for compiled C++ binaries.

**Steps:**

1. **Create Railway Account**
   - Visit https://railway.app
   - Sign up with GitHub

2. **Connect Repository**
   ```bash
   railway login
   railway link
   ```

3. **Deploy with Dockerfile**
   - Railway automatically detects `Dockerfile`
   - No configuration needed
   - Binary is built and hosted

4. **Set Environment**
   ```bash
   railway variables set NODE_ENV=production
   railway up
   ```

5. **Get Public URL**
   - Railway assigns: `spectra-vision-api.railway.app`
   - API accessible at: `https://spectra-vision-api.railway.app/api/v1/emotion`

### Option 2: Docker Hub + AWS EC2

**Build Docker Image:**
```bash
docker build -t yourusername/spectra-vision:latest .
docker push yourusername/spectra-vision:latest
```

**Run on AWS EC2:**
```bash
#!/bin/bash
docker pull yourusername/spectra-vision:latest
docker run -d -p 8080:8080 yourusername/spectra-vision:latest
```

### Option 3: Heroku (if C++ buildpack added)

```bash
git push heroku main
heroku open /health
```

### Option 4: Manual VPS (DigitalOcean, Linode)

**Build on the server:**
```bash
ssh user@vps-ip
git clone https://github.com/yourusername/spectra-vision.git
cd spectra-vision/backend
mkdir build && cd build
cmake -G "Ninja" ..
cmake --build .
./bin/SpectraVision &
```

---

## Frontend Deployment

### Option 1: Vercel (Recommended)

Perfect for Next.js/React applications.

**Setup:**

1. **Create `frontend` directory with Next.js**
   ```bash
   npx create-next-app@latest frontend
   ```

2. **Configure API proxy in `vercel.json`**
   ```json
   {
     "routes": [
       {
         "src": "/api/(.*)",
         "dest": "https://spectra-vision-api.railway.app/api/$1"
       }
     ]
   }
   ```

3. **Deploy to Vercel**
   ```bash
   npm install -g vercel
   cd frontend
   vercel
   ```

4. **Set Environment Variables**
   ```bash
   vercel env add API_URL https://spectra-vision-api.railway.app
   ```

### Option 2: Netlify

**Deploy:**
```bash
npm install -g netlify-cli
netlify deploy --prod
```

### Option 3: GitHub Pages (Static Only)

```bash
npm run build
npm run export
# Push `out` directory to gh-pages branch
```

---

## Full Stack Deployment

### Architecture

```
┌────────────────────────────────────┐
│         Vercel Frontend            │
│    (React/Next.js UI)              │
│  https://spectra-vision.vercel.app │
└────────────┬───────────────────────┘
             │ HTTPS API Calls
             ↓
┌────────────────────────────────────┐
│       Railway Backend              │
│    (C++ REST Server)               │
│ https://spectra-vision-api.rail... │
└────────────────────────────────────┘
```

### Deployment Steps

**1. Deploy Backend First**

```bash
# Create Railway project
railway init

# Deploy
railway up

# Get public URL
railway variables get RAILWAY_DOMAIN
# Output: spectra-vision-api.railway.app
```

**2. Configure Frontend**

```bash
# In frontend/.env.local
NEXT_PUBLIC_API_URL=https://spectra-vision-api.railway.app

# Or in vercel.json for production
"env": {
  "NEXT_PUBLIC_API_URL": {
    "default": "https://spectra-vision-api.railway.app"
  }
}
```

**3. Deploy Frontend**

```bash
cd frontend
vercel --prod

# Set environment variables in Vercel dashboard
```

**4. Test Integration**

```bash
# Frontend makes request to backend
curl https://spectra-vision.vercel.app/api/health
# Proxied to:
curl https://spectra-vision-api.railway.app/health
```

---

## Environment Variables

### Backend (.env)

```env
LOG_LEVEL=INFO
SERVER_PORT=8080
SERVER_HOST=0.0.0.0
CORS_ENABLED=true
CORS_ORIGIN=*
```

### Frontend (.env.local / vercel.json)

```env
NEXT_PUBLIC_API_URL=https://spectra-vision-api.railway.app
NEXT_PUBLIC_LOG_LEVEL=info
```

---

## Monitoring & Debugging

### Backend Logs

**Railway:**
```bash
railway logs
railway logs -f  # Follow
```

**Local Docker:**
```bash
docker logs --follow container-id
```

### Health Checks

**Manual:**
```bash
curl https://spectra-vision-api.railway.app/health
```

**Automated (Monitoring):**
```bash
# Add to Railway with interval
curl -f https://spectra-vision-api.railway.app/health || exit 1
```

### Performance Monitoring

**Add metrics endpoint (future):**
```cpp
svr.Get("/metrics", [](const httplib::Request&, httplib::Response& res) {
    res.set_content(prometheusMetrics(), "text/plain");
});
```

---

## CI/CD Pipeline

### GitHub Actions

Workflow file: `.github/workflows/build.yml`

**Automatic on push:**
1. Build on Ubuntu/Windows/macOS
2. Run C++ static analysis (cppcheck)
3. Test with different C++ standards (17, 20)

**Deploy on successful build:**
```yaml
- name: Deploy to Railway
  if: github.ref == 'refs/heads/main'
  run: |
    railway login --token ${{ secrets.RAILWAY_TOKEN }}
    railway up
```

---

## Scaling Considerations

### Backend

**Current:** Single process on Railway

**Future options:**
- Horizontal scaling with load balancer
- Kubernetes deployment (Docker)
- Lambda functions (if serverless)

### Frontend

**Vercel automatically:**
- Distributes globally (CDN)
- Auto-scales
- Caches assets

---

## Security Checklist

- [ ] Enable HTTPS (automatic on Vercel/Railway)
- [ ] Set CORS headers appropriately
- [ ] Add API rate limiting
- [ ] Validate all inputs
- [ ] Use environment variables for secrets
- [ ] Enable logging and monitoring
- [ ] Regular dependency updates

---

## Cost Estimation

**Railway (Backend):**
- Free tier: $5/month
- Production: ~$10-50/month depending on load

**Vercel (Frontend):**
- Free: Generous free tier
- Pro: $20/month

**Total:** $15-70/month for full production deployment

---

## Rollback Procedure

### Railway

```bash
# List deployments
railway deployments

# Rollback to previous
railway deployments rollback <deployment-id>
```

### Vercel

Dashboard → Deployments → Select previous → Promote to Production

---

## Troubleshooting

### Backend won't start

**Check logs:**
```bash
railway logs --tail 50
```

**Common issues:**
- Port already in use → Change port in code
- Missing dependencies → Check Dockerfile
- C++ compilation error → Local build first

### API calls failing

**Check:**
```bash
# Backend is running
curl https://spectra-vision-api.railway.app/health

# CORS is enabled
curl -H "Origin: https://spectra-vision.vercel.app" \
  https://spectra-vision-api.railway.app/health
```

### Frontend can't reach backend

**Check:**
1. Backend URL in environment variable
2. CORS headers in backend
3. Network firewall rules

---

## Local Development

**Backend:**
```bash
cd backend/build
./bin/SpectraVision
# Listens on http://localhost:8080
```

**Frontend (separate terminal):**
```bash
cd frontend
npm run dev
# Listens on http://localhost:3000
# Proxies /api to http://localhost:8080
```

---

## Next Steps

1. ✅ Build backend locally
2. ✅ Deploy to Railway
3. ✅ Create frontend
4. ✅ Deploy to Vercel
5. ⏳ Add authentication
6. ⏳ Add monitoring/alerting
7. ⏳ Scale as needed

---

## Support

For deployment issues, check:
- Railway docs: https://docs.railway.app
- Vercel docs: https://vercel.com/docs
- GitHub Actions: https://github.com/features/actions
