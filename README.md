# Custom TTree-Like Structure

> Data processing and anomaly detection for High Energy Physics using Apache Arrow and ROOT made for educational purposes.

## 🔍 Overview

This project implements a high-performance data processing pipeline for high-energy physics (HEP) experiments, integrating Apache Arrow with ROOT data formats and applying machine learning techniques for analysing datasets (currently does not support ATLAS and ALICE datasets).

### Key Features

- **Custom TTree Implementation**: Modern C++ bridge between ROOT and Apache Arrow
- **Efficient Data Processing**: Optimized for HEP's large-scale datasets (12M+ events)
- **Efficient Data Conversion**: ROOT to Arrow format conversion preserving physics information
- **ML-based Anomaly Detection**: Uses Isolation Forest to identify outliers in scattering data
- **High Performance**: C++ and Python hybrid approach matching CERN's real-world data pipelines
- **Optimized Storage & Retrieval**: Column-oriented architecture for efficient data access patterns

## 🚀 Motivation

Modern high-energy physics experiments generate hundreds of petabytes of data. Which creates challenges in:

1. Data storage and transmission
2. Processing efficiency

This project addresses these challenges by combining:
- Columnar data processing (Apache Arrow)
- Custom data structures that can be extended and optimized for physics
- Machine learning for automated anomaly detection and storage optimization

## 📋 Current Results

- Successfully converted ROOT TTree data to Apache Arrow format
- Implemented initial anomaly detection using Isolation Forest
- Achieved efficient processing of 12M+ event datasets
- Preserved physics information while optimizing storage
  
## 🔮 Future Work

### In Progress

1. **Multiple Anomaly Detection Algorithms**
   - Implementing DBSCAN for density-based clustering
   - Adding Autoencoders for non-linear feature learning
   - Developing Deep SVDD for one-class classification

2. **Physics-Informed Features**
   - Adding domain-specific feature engineering
   - Incorporating physics constraints into anomaly detection

3. **Performance Benchmarking**
   - Comparing algorithm performance across different physics phenomena
   - Optimizing for CERN's computing infrastructure

### Planned

1. **Advanced Visualization Dashboard**
   - Interactive exploration of detected anomalies
   - Physics-meaningful data representations

2. **ML-based Compression**
   - Develop lossy compression with physics-aware precision
   - Train AI to recover precision from compressed data
   - Implement adaptive precision based on physics significance

3. **Real-time Analysis Capabilities**
   - Adapt algorithms for streaming data
   - Optimize for trigger-level decisions

4. **Enhanced Data Access Patterns**
   - Implement columnar caching for frequently accessed variables
   - Develop predictive prefetching based on analysis patterns
   - Create specialized indexes for physics-based queries

## 🛠️ Installation and Usage

### Prerequisites
- ROOT (version 6.20+)
- Apache Arrow (version 7.0.0+)
- Python 3.8+
- PyArrow, NumPy, scikit-learn

### Building from Source
```bash
git clone https://github.com/haveheartt/ttree-like-ds-cern.git
cd ttree-like-ds-cern
make
```

### Running the Pipeline
```bash
  todo
```

## 🔗 Relevance to CERN

This project directly addresses key challenges for the LHC High-Luminosity phase:

1. **Data Volume**: Efficient storage and processing techniques
2. **Anomaly Detection**: ML-based approaches to identify rare physics events
3. **Modern Tools**: Integration of HEP-specific formats with industry-standard data science tools
4. **Storage Optimization**: Reducing storage requirements while preserving physics information
5. **Analysis Acceleration**: Faster time-to-insight for physics analyses
