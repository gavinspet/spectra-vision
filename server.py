#!/usr/bin/env python3
"""
Spectra Vision - Python HTTP Server
Minimal Flask-based server for emotion detection API
"""

from flask import Flask, request, jsonify
import base64
import logging
from datetime import datetime

app = Flask(__name__)

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger(__name__)

# Mock emotion detection service
class EmotionDetectionService:
    """Mock service that returns test emotions"""
    
    @staticmethod
    def detect_emotion(model_id: str, image_data: str) -> dict:
        """
        Simulate emotion detection on an image
        
        Args:
            model_id: Model identifier
            image_data: Base64 encoded image data
        
        Returns:
            Dictionary with detection results
        """
        logger.info(f"Detecting emotion with model: {model_id}")
        
        return {
            "classLabel": "Happy",
            "classId": 0,
            "confidence": 0.94,
            "inferenceTimeMs": 42,
            "timestamp": datetime.now().isoformat()
        }


@app.route('/health', methods=['GET'])
def health_check():
    """Health check endpoint"""
    return jsonify({
        "status": "ok",
        "service": "Spectra Vision",
        "version": "1.0.0"
    }), 200


@app.route('/api/v1/emotion', methods=['POST'])
def detect_emotion():
    """
    POST /api/v1/emotion
    Detect emotions in an image
    
    Request JSON:
    {
        "modelId": "model-name",
        "imageBase64": "base64-encoded-image",
        "confidenceThreshold": 0.5,
        "maxResults": 5
    }
    
    Response JSON:
    {
        "classLabel": "Happy",
        "classId": 0,
        "confidence": 0.94,
        "inferenceTimeMs": 42
    }
    """
    try:
        data = request.get_json()
        
        # Validate required fields
        if not data:
            return jsonify({"error": "Request body must be JSON"}), 400
        
        model_id = data.get('modelId')
        image_data = data.get('imageBase64')
        
        if not model_id:
            return jsonify({"error": "modelId is required"}), 400
        if not image_data:
            return jsonify({"error": "imageBase64 is required"}), 400
        
        # Call emotion detection service
        service = EmotionDetectionService()
        result = service.detect_emotion(model_id, image_data)
        
        return jsonify(result), 200
        
    except Exception as e:
        logger.error(f"Error in emotion detection: {str(e)}")
        return jsonify({"error": str(e)}), 500


@app.route('/', methods=['GET'])
def index():
    """Welcome endpoint"""
    return jsonify({
        "name": "Spectra Vision Server",
        "version": "1.0.0",
        "endpoints": {
            "health": "GET /health",
            "emotion_detection": "POST /api/v1/emotion"
        }
    }), 200


@app.errorhandler(404)
def not_found(error):
    """Handle 404 errors"""
    return jsonify({"error": "Endpoint not found"}), 404


@app.errorhandler(500)
def internal_error(error):
    """Handle 500 errors"""
    return jsonify({"error": "Internal server error"}), 500


if __name__ == '__main__':
    logger.info("=" * 60)
    logger.info("Spectra Vision Server")
    logger.info("=" * 60)
    logger.info("Starting server on 0.0.0.0:8080")
    logger.info("Health check: GET http://localhost:8080/health")
    logger.info("Emotion detection: POST http://localhost:8080/api/v1/emotion")
    logger.info("=" * 60)
    
    app.run(host='0.0.0.0', port=8080, debug=False)
