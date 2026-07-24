# Spectra Vision Frontend

Modern React/Next.js frontend for the Spectra Vision emotion detection API.

## Features

- ✅ Real-time emotion detection requests
- ✅ Multiple model selection (mock, advanced)
- ✅ Live server status indicator
- ✅ Beautiful gradient UI
- ✅ Responsive design
- ✅ API error handling
- ✅ Deployed to Vercel

## Quick Start (Local Development)

### Prerequisites
- Node.js 16+
- Python backend running on http://localhost:8080

### Install & Run

```bash
cd frontend
npm install
npm run dev
```

Visit: http://localhost:3000

## Deployment

### Deploy to Vercel

```bash
npm install -g vercel
vercel login
vercel deploy
```

### Environment Variables (Vercel)

Set in Vercel dashboard:

```
NEXT_PUBLIC_API_URL=https://spectra-vision-api.railway.app
```

## Build

```bash
npm run build
npm start
```

## Project Structure

```
frontend/
├── pages/
│   └── index.js          # Main emotion detection page
├── styles.css            # Global styles
├── next.config.js        # Next.js configuration
├── package.json          # Dependencies
├── .env.local            # Local development env vars
└── README.md             # This file
```

## API Integration

Connects to backend at: `process.env.NEXT_PUBLIC_API_URL`

Default: `http://localhost:8080`

Endpoints:
- `GET /health` - Server status
- `POST /api/v1/emotion` - Emotion detection
- `GET /api/v1/models` - List models

## Technologies

- **Framework**: Next.js 14
- **Library**: React 18
- **Styling**: CSS-in-JS (styled components)
- **Deployment**: Vercel
- **Language**: JavaScript

## License

MIT
