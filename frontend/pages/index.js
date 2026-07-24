import { useState, useEffect } from 'react'
import Head from 'next/head'
import styles from './styles.module.css'

const API_URL = process.env.NEXT_PUBLIC_API_URL || 'http://localhost:8080'

const EMOTION_COLORS = {
  'Happy': '#FFD93D',
  'Sad': '#6C5CE7',
  'Angry': '#E74C3C',
  'Surprised': '#FD79A8',
  'Neutral': '#95A5A6'
}

const EMOTION_ICONS = {
  'Happy': '😊',
  'Sad': '😢',
  'Angry': '😠',
  'Surprised': '😮',
  'Neutral': '😐'
}

export default function Home() {
  const [modelId, setModelId] = useState('emotion-v1')
  const [imageData, setImageData] = useState('test-image-data')
  const [response, setResponse] = useState(null)
  const [loading, setLoading] = useState(false)
  const [serverStatus, setServerStatus] = useState('checking')

  useEffect(() => {
    checkServer()
    const interval = setInterval(checkServer, 30000)
    return () => clearInterval(interval)
  }, [])

  const checkServer = async () => {
    try {
      const res = await fetch(`${API_URL}/health`, { timeout: 5000 })
      setServerStatus(res.ok ? 'online' : 'offline')
    } catch {
      setServerStatus('offline')
    }
  }

  const handleEmotionDetection = async (e) => {
    e.preventDefault()
    setLoading(true)
    setResponse(null)

    try {
      const res = await fetch(`${API_URL}/api/v1/emotion`, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({
          modelId,
          imageBase64: imageData
        })
      })

      const data = await res.json()
      setResponse({
        status: res.ok ? 'success' : 'error',
        data
      })
      if (res.ok) setServerStatus('online')
    } catch (error) {
      setResponse({
        status: 'error',
        data: null,
        error: error.message
      })
      setServerStatus('offline')
    } finally {
      setLoading(false)
    }
  }

  return (
    <>
      <Head>
        <title>Spectra Vision - Emotion Detection</title>
        <meta name="description" content="Advanced AI Emotion Detection Engine" />
      </Head>

      <div className="wrapper">
        <div className="background"></div>
        
        <div className="container">
          <div className="header">
            <div className="logo">🎭</div>
            <h1>Spectra Vision</h1>
            <p>Advanced AI Emotion Detection Engine</p>
            <div className={`status-badge ${serverStatus}`}>
              <span className="dot"></span>
              {serverStatus === 'online' ? 'Service Online' : 'Service Connecting...'}
            </div>
          </div>

          <div className="main-content">

            <div className="form-group">
              <label htmlFor="modelId">Detection Model</label>
              <select
                id="modelId"
                value={modelId}
                onChange={(e) => setModelId(e.target.value)}
              >
                <option value="emotion-v1">Emotion V1 - Fast</option>
                <option value="emotion-v2">Emotion V2 - Advanced</option>
                <option value="mock">Mock Detector</option>
                <option value="advanced">Advanced Engine</option>
              </select>
            </div>

            <div className="form-group">
              <label htmlFor="imageData">Image Input</label>
              <textarea
                id="imageData"
                value={imageData}
                onChange={(e) => setImageData(e.target.value)}
                placeholder="Enter base64 encoded image or image description..."
              />
            </div>

            <button 
              type="submit" 
              disabled={loading}
              className={loading ? 'loading' : ''}
            >
              {loading ? (
                <>
                  <span className="spinner"></span>
                  Analyzing...
                </>
              ) : (
                '✨ Detect Emotion'
              )}
            </button>
          </form>

          {response && (
            <div className={`result-card ${response.status}`}>
              {response.status === 'success' ? (
                <>
                  <div className="emotion-display">
                    <div className="emotion-icon">
                      {EMOTION_ICONS[response.data.classLabel] || '🎭'}
                    </div>
                    <div className="emotion-info">
                      <h3>{response.data.classLabel}</h3>
                      <div className="confidence-bar">
                        <div 
                          className="confidence-fill" 
                          style={{ 
                            width: `${response.data.confidence * 100}%`,
                            backgroundColor: EMOTION_COLORS[response.data.classLabel] || '#667eea'
                          }}
                        ></div>
                      </div>
                      <p className="confidence-text">
                        Confidence: {(response.data.confidence * 100).toFixed(1)}%
                      </p>
                    </div>
                  </div>
                  <div className="metadata">
                    <div className="meta-item">
                      <span className="meta-label">Processing Time</span>
                      <span className="meta-value">{response.data.inferenceTimeMs}ms</span>
                    </div>
                    <div className="meta-item">
                      <span className="meta-label">Timestamp</span>
                      <span className="meta-value">{new Date(response.data.timestamp).toLocaleTimeString()}</span>
                    </div>
                  </div>
                </>
              ) : (
                <div className="error-message">
                  <p>⚠️ {response.error || 'Detection failed. Please try again.'}</p>
                </div>
              )}
            </div>
          )}
        </div>

        <div className="features-grid">
          <div className="feature-card">
            <div className="feature-icon">⚡</div>
            <h3>Lightning Fast</h3>
            <p>Real-time emotion detection in milliseconds</p>
          </div>
          <div className="feature-card">
            <div className="feature-icon">🎯</div>
            <h3>High Accuracy</h3>
            <p>Advanced ML models with 94%+ confidence</p>
          </div>
          <div className="feature-card">
            <div className="feature-icon">🔒</div>
            <h3>Enterprise Ready</h3>
            <p>Production-grade C++ backend with design patterns</p>
          </div>
        </div>
      </div>

      <style jsx global>{`
        * {
          margin: 0;
          padding: 0;
          box-sizing: border-box;
        }

        html, body {
          font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', 'Roboto', 'Oxygen', 'Ubuntu', 'Cantarell', sans-serif;
          background: #0f0f1e;
          color: #fff;
          overflow-x: hidden;
        }

        .wrapper {
          position: relative;
          min-height: 100vh;
          background: #0f0f1e;
        }

        .background {
          position: fixed;
          top: 0;
          left: 0;
          width: 100%;
          height: 100%;
          background: linear-gradient(135deg, #667eea 0%, #764ba2 25%, #f093fb 50%, #4facfe 75%, #00f2fe 100%);
          background-size: 400% 400%;
          animation: gradient 15s ease infinite;
          z-index: -1;
          opacity: 0.15;
        }

        @keyframes gradient {
          0% { background-position: 0% 50%; }
          50% { background-position: 100% 50%; }
          100% { background-position: 0% 50%; }
        }

        .container {
          max-width: 1000px;
          margin: 0 auto;
          padding: 40px 20px;
          position: relative;
          z-index: 1;
        }

        .header {
          text-align: center;
          margin-bottom: 50px;
          animation: fadeInDown 0.8s ease-out;
        }

        @keyframes fadeInDown {
          from {
            opacity: 0;
            transform: translateY(-30px);
          }
          to {
            opacity: 1;
            transform: translateY(0);
          }
        }

        .logo {
          font-size: 60px;
          margin-bottom: 20px;
          display: inline-block;
          animation: bounce 2s ease-in-out infinite;
        }

        @keyframes bounce {
          0%, 100% { transform: translateY(0); }
          50% { transform: translateY(-10px); }
        }

        .header h1 {
          font-size: 3.5em;
          font-weight: 800;
          margin-bottom: 10px;
          background: linear-gradient(135deg, #667eea 0%, #f093fb 100%);
          -webkit-background-clip: text;
          -webkit-text-fill-color: transparent;
          background-clip: text;
        }

        .header p {
          font-size: 1.3em;
          color: #b0b0ff;
          margin-bottom: 20px;
        }

        .status-badge {
          display: inline-flex;
          align-items: center;
          gap: 8px;
          padding: 10px 20px;
          border-radius: 50px;
          font-size: 0.9em;
          font-weight: 600;
          backdrop-filter: blur(10px);
          border: 1px solid rgba(255, 255, 255, 0.2);
          transition: all 0.3s ease;
        }

        .status-badge.online {
          background: rgba(76, 175, 80, 0.2);
          color: #4caf50;
        }

        .status-badge.offline {
          background: rgba(244, 67, 54, 0.2);
          color: #ff6b6b;
        }

        .status-badge .dot {
          width: 8px;
          height: 8px;
          border-radius: 50%;
          background: currentColor;
          animation: pulse 2s ease-in-out infinite;
        }

        @keyframes pulse {
          0%, 100% { opacity: 1; }
          50% { opacity: 0.5; }
        }

        .main-content {
          display: grid;
          gap: 30px;
        }

        .card {
          background: rgba(30, 30, 50, 0.8);
          backdrop-filter: blur(20px);
          border: 1px solid rgba(255, 255, 255, 0.1);
          border-radius: 20px;
          padding: 40px;
          animation: fadeInUp 0.8s ease-out;
          box-shadow: 0 20px 60px rgba(0, 0, 0, 0.3);
        }

        @keyframes fadeInUp {
          from {
            opacity: 0;
            transform: translateY(30px);
          }
          to {
            opacity: 1;
            transform: translateY(0);
          }
        }

        .card h2 {
          font-size: 1.8em;
          margin-bottom: 30px;
          color: #fff;
          font-weight: 700;
        }

        .form-group {
          margin-bottom: 25px;
        }

        .form-group label {
          display: block;
          margin-bottom: 10px;
          font-size: 0.95em;
          font-weight: 600;
          color: #b0b0ff;
        }

        .form-group input,
        .form-group textarea,
        .form-group select {
          width: 100%;
          padding: 14px 16px;
          background: rgba(255, 255, 255, 0.05);
          border: 1px solid rgba(255, 255, 255, 0.1);
          border-radius: 10px;
          color: #fff;
          font-size: 0.95em;
          font-family: inherit;
          transition: all 0.3s ease;
        }

        .form-group input::placeholder,
        .form-group textarea::placeholder {
          color: rgba(255, 255, 255, 0.5);
        }

        .form-group input:focus,
        .form-group textarea:focus,
        .form-group select:focus {
          outline: none;
          background: rgba(102, 126, 234, 0.1);
          border-color: #667eea;
          box-shadow: 0 0 0 2px rgba(102, 126, 234, 0.2);
        }

        .form-group textarea {
          min-height: 120px;
          resize: vertical;
        }

        button {
          width: 100%;
          padding: 16px;
          background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
          color: white;
          border: none;
          border-radius: 10px;
          font-size: 1em;
          font-weight: 700;
          cursor: pointer;
          transition: all 0.3s ease;
          display: flex;
          align-items: center;
          justify-content: center;
          gap: 8px;
          margin-top: 10px;
        }

        button:hover:not(:disabled) {
          transform: translateY(-3px);
          box-shadow: 0 15px 40px rgba(102, 126, 234, 0.4);
        }

        button:disabled {
          opacity: 0.7;
          cursor: not-allowed;
        }

        .spinner {
          display: inline-block;
          width: 16px;
          height: 16px;
          border: 2px solid rgba(255, 255, 255, 0.3);
          border-top-color: white;
          border-radius: 50%;
          animation: spin 0.8s linear infinite;
        }

        @keyframes spin {
          to { transform: rotate(360deg); }
        }

        .result-card {
          background: rgba(255, 255, 255, 0.05);
          border: 1px solid rgba(255, 255, 255, 0.1);
          border-radius: 15px;
          padding: 30px;
          margin-top: 20px;
          animation: fadeIn 0.5s ease-out;
        }

        @keyframes fadeIn {
          from { opacity: 0; }
          to { opacity: 1; }
        }

        .result-card.error {
          border-color: rgba(255, 100, 100, 0.3);
          background: rgba(255, 100, 100, 0.05);
        }

        .emotion-display {
          display: flex;
          align-items: center;
          gap: 30px;
          margin-bottom: 25px;
        }

        .emotion-icon {
          font-size: 80px;
          flex-shrink: 0;
        }

        .emotion-info h3 {
          font-size: 2em;
          margin-bottom: 15px;
          color: #fff;
        }

        .confidence-bar {
          width: 100%;
          height: 10px;
          background: rgba(255, 255, 255, 0.1);
          border-radius: 10px;
          overflow: hidden;
          margin-bottom: 10px;
        }

        .confidence-fill {
          height: 100%;
          transition: width 0.5s ease;
          border-radius: 10px;
        }

        .confidence-text {
          color: #b0b0ff;
          font-size: 0.95em;
          font-weight: 600;
        }

        .metadata {
          display: grid;
          grid-template-columns: repeat(auto-fit, minmax(150px, 1fr));
          gap: 15px;
          padding-top: 20px;
          border-top: 1px solid rgba(255, 255, 255, 0.1);
        }

        .meta-item {
          display: flex;
          flex-direction: column;
        }

        .meta-label {
          color: rgba(255, 255, 255, 0.6);
          font-size: 0.85em;
          font-weight: 600;
          text-transform: uppercase;
          letter-spacing: 0.5px;
          margin-bottom: 5px;
        }

        .meta-value {
          color: #fff;
          font-size: 1.1em;
          font-weight: 700;
          font-family: 'Monaco', 'Courier New', monospace;
        }

        .error-message {
          text-align: center;
          padding: 20px;
          color: #ff6b6b;
          font-size: 1.05em;
        }

        .features-grid {
          display: grid;
          grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));
          gap: 20px;
          margin-top: 50px;
        }

        .feature-card {
          background: rgba(30, 30, 50, 0.8);
          backdrop-filter: blur(20px);
          border: 1px solid rgba(255, 255, 255, 0.1);
          border-radius: 15px;
          padding: 30px;
          text-align: center;
          transition: all 0.3s ease;
          animation: fadeInUp 0.8s ease-out;
        }

        .feature-card:hover {
          border-color: rgba(102, 126, 234, 0.5);
          transform: translateY(-5px);
          box-shadow: 0 10px 30px rgba(102, 126, 234, 0.2);
        }

        .feature-icon {
          font-size: 40px;
          margin-bottom: 15px;
        }

        .feature-card h3 {
          font-size: 1.2em;
          margin-bottom: 10px;
          color: #fff;
        }

        .feature-card p {
          color: #b0b0ff;
          font-size: 0.95em;
        }

        @media (max-width: 768px) {
          .container {
            padding: 20px 15px;
          }

          .header h1 {
            font-size: 2.5em;
          }

          .emotion-display {
            flex-direction: column;
            text-align: center;
          }

          .emotion-icon {
            font-size: 60px;
          }

          .emotion-info h3 {
            font-size: 1.5em;
          }
        }
      `}</style>
    </>
  )
}
