#include <algorithm>
#include <QApplication>
#include <QList>
#include <QMap>
#include <QMultiMap>
#include <QPainter>
#include <QSet>
#include <QSvgGenerator>
#include <boloq.h>
#include <boloq/io.h>
using namespace boloq;

using node_ptr = typename boolean_function::node_ptr;
using label_type = typename node_ptr::element_type::label_type;
using index_type = typename node_ptr::element_type::index_type;

const int WIDTH = 640;
const int HEIGHT = 640;
const int R = 40;

class Graph {
    // 順序通りにソートされたラベル
    QList<label_type> labels;
    // ラベルごとに行で分けたノード
    QList<QList<node_ptr>> nodes;
    // 行内のインデックス
    QMap<index_type, int> indexes;

public:
    Graph(const QMultiMap<label_type, node_ptr>& labelNodes) :
        labels(labelNodes.keys())
    {
        qSort(labels);
        labels.erase(std::unique(labels.begin(), labels.end()), labels.end());

        for (const label_type& label : labels) {
            const auto& nodeList = labelNodes.values(label);
            nodes.append(nodeList);
            for (int i = 0; i < nodeList.size(); ++i) {
                indexes.insert(nodeList[i]->index(), i);
            }
        }
    }

    int getIndex(const node_ptr& node) {return indexes[node->index()];}
    const QList<QList<node_ptr>>& getNodes() {return nodes;}
    int labelIndex(const label_type& label) {
        auto pos = std::lower_bound(labels.begin(), labels.end(), label);
        return std::distance(labels.begin(), pos);
    }
};

class GraphGenerateVisitor {
    QMultiMap<label_type, node_ptr> nodes;
    QSet<index_type> visited;

public:
    using result_type = void;

    GraphGenerateVisitor() {}

    void operator()(const node_ptr& node) {
        const auto& index = node->index();
        const auto& label = node->label();
        if (visited.contains(index)) return;
        visited.insert(index);
        nodes.insert(label, node);

        if (!node->is_terminal()) {
            node->then_node()->accept(*this);
            node->else_node()->accept(*this);
        }
    }

    Graph generate() {
        return Graph(nodes);
    }
};

QList<QList<QPoint>> getPoints(const QList<QList<node_ptr>>& nodes) {
    QList<QList<QPoint>> result;
    const int yInterval = HEIGHT / (nodes.size() + 1);
    int y = yInterval;
    for (const auto& nodeList : nodes) {
        QList<QPoint> pointList;
        int length = nodeList.size();
        const int xInterval = WIDTH / (length + 1);
        int x = xInterval;
        for (int i = 0; i < length; ++i) {
            pointList.append(QPoint(x, y));
            x += xInterval;
        }
        result.append(pointList);
        y += yInterval;
    }
    return result;
}

template<class Func>
void traverseNodes(const QList<QList<node_ptr>>& nodes, const QList<QList<QPoint>>& points, Func f) {
    for (int i = 0; i < nodes.size(); ++i) {
        for (int j = 0; j < nodes[i].size(); ++j) {
            f(nodes[i][j], points[i][j]);
        }
    }
}

int main(int argc, char **argv) {
    QApplication app(argc, argv);

    QSvgGenerator generator;
    generator.setFileName("bdd.svg");
    generator.setViewBox(QRect(0, 0, WIDTH, HEIGHT));
    generator.setTitle("BDD");

    boolean_function x('x');
    boolean_function y('y');
    auto f = x & y;
    std::cout << f << std::endl;

    GraphGenerateVisitor visitor;
    f.accept(visitor);
    Graph graph = visitor.generate();
    const QList<QList<node_ptr>>& nodes = graph.getNodes();
    const QList<QList<QPoint>>& points = getPoints(nodes);

    QPainter painter(&generator);
    // Then edges(solid)
    traverseNodes(nodes, points, [&](const node_ptr& node, const QPoint& point) {
        if (!node->is_terminal()) {
            const int thenNextV = graph.getIndex(node->then_node());
            const int thenNextH = graph.labelIndex(node->then_node()->label());
            painter.drawLine(point, points[thenNextH][thenNextV]);
        }
    });
    // Else edges(dash)
    painter.setPen(Qt::DashLine);
    traverseNodes(nodes, points, [&](const node_ptr& node, const QPoint& point) {
        if (!node->is_terminal()) {
            const int thenNextV = graph.getIndex(node->else_node());
            const int thenNextH = graph.labelIndex(node->else_node()->label());
            painter.drawLine(point, points[thenNextH][thenNextV]);
        }
    });
    // nodes
    painter.setPen(Qt::SolidLine);
    traverseNodes(nodes, points, [&](const node_ptr& node, const QPoint& point) {
        painter.setBrush(Qt::white);
        const int x = point.x() - R / 2;
        const int y = point.y() - R / 2;
        if (node->is_terminal()) {
            painter.drawRect(x, y, R, R);
            painter.drawText(x, y, R, R, Qt::AlignCenter, (node->index() == 1) ? "1" : "0");
        }
        else {
            painter.drawEllipse(x, y, R, R);
            QString labelStr = QChar(static_cast<char>(node->label()));
            painter.drawText(x, y, R, R, Qt::AlignCenter, labelStr);
        }
    });
    return 0;
}

