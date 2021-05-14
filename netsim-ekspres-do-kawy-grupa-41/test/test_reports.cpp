#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "factory.hpp"
#include "reports.hpp"

#include <functional>

using ::testing::ContainerEq;

void perform_report_check(std::function<void(std::ostringstream&)>& reporting_function,
                          std::vector<std::string>& expected_report_lines) {
    // Wygeneruj raport.
    std::ostringstream report_oss;
    reporting_function(report_oss);

    std::vector<std::string> output_lines;
    std::istringstream iss;
    iss.str(report_oss.str());
    std::string line;
    while (std::getline(iss, line, '\n')) {
        output_lines.push_back(line);
    }

    std::ostringstream expected_report_oss;
    for (auto& line : expected_report_lines) {
        expected_report_oss << line << "\n";
    }

    ASSERT_EQ(output_lines.size(), output_lines.size());
    std::string delimiter = "----";
    EXPECT_THAT(output_lines, ContainerEq(expected_report_lines))
                        << "Actual (\"" << delimiter << "\" were added for readability):\n"
                        << delimiter << "\n" << report_oss.str() << delimiter << "\n"
                        << "Expected (\"" << delimiter << "\" were added for readability):\n"
                        << delimiter << "\n" << expected_report_oss.str() << delimiter << "\n";

    // Dodane dla czytelności.
    for (std::size_t i = 0; i < output_lines.size(); ++i) {
        ASSERT_EQ(output_lines[i], expected_report_lines[i]) << "(in element no.: " << (i + 1) << ")";
    }
}

void perform_turn_report_check(const Factory& factory, Time t, std::vector<std::string>& expected_report_lines) {
    std::function<void(std::ostringstream&)> reporting_function = [&factory, t](
            std::ostringstream& oss) { generate_simulation_turn_report(factory, oss, t); };
    perform_report_check(reporting_function, expected_report_lines);
}

void perform_structure_report_check(const Factory& factory, std::vector<std::string>& expected_report_lines) {
    std::function<void(std::ostringstream&)> reporting_function = [&factory](
            std::ostringstream& oss) { generate_structure_report(factory, oss); };
    perform_report_check(reporting_function, expected_report_lines);
}


TEST(ReportsTest, StructureReport_R1W1S1) {
    // Utwórz fabrykę.
    Factory factory;

    factory.add_ramp(Ramp(1, 1));
    factory.add_worker(Worker(1, 1, std::make_unique<PackageQueue>(PackageQueueType::FIFO)));
    factory.add_storehouse(Storehouse(1));

    Ramp& r = *(factory.find_ramp_by_id(1));
    r.receiver_preferences_.add_receiver(&(*factory.find_worker_by_id(1)));

    Worker& w = *(factory.find_worker_by_id(1));
    w.receiver_preferences_.add_receiver(&(*factory.find_storehouse_by_id(1)));

    // -----------------------------------------------------------------------

    std::vector<std::string> expected_report_lines{
            "",
            "== LOADING RAMPS ==",
            "",
            "LOADING RAMP #1",
            "  Delivery interval: 1",
            "  Receivers:",
            "    worker #1",
            "",
            "",
            "== WORKERS ==",
            "",
            "WORKER #1",
            "  Processing time: 1",
            "  Queue type: FIFO",
            "  Receivers:",
            "    storehouse #1",
            "",
            "",
            "== STOREHOUSES ==",
            "",
            "STOREHOUSE #1",
            "",
    };

    perform_structure_report_check(factory, expected_report_lines);
}

TEST(ReportsTest, StructureReport_R2W2S2) {
    // Utwórz fabrykę.
    Factory factory;

    factory.add_ramp(Ramp(1, 1));
    factory.add_ramp(Ramp(2, 2));
    factory.add_worker(Worker(1, 1, std::make_unique<PackageQueue>(PackageQueueType::FIFO)));
    factory.add_worker(Worker(2, 2, std::make_unique<PackageQueue>(PackageQueueType::LIFO)));
    factory.add_storehouse(Storehouse(1));
    factory.add_storehouse(Storehouse(2));

    Ramp& r1 = *(factory.find_ramp_by_id(1));
    r1.receiver_preferences_.add_receiver(&(*factory.find_worker_by_id(1)));

    Ramp& r2 = *(factory.find_ramp_by_id(2));
    r2.receiver_preferences_.add_receiver(&(*factory.find_worker_by_id(1)));
    r2.receiver_preferences_.add_receiver(&(*factory.find_worker_by_id(2)));

    Worker& w1 = *(factory.find_worker_by_id(1));
    w1.receiver_preferences_.add_receiver(&(*factory.find_storehouse_by_id(1)));

    Worker& w2 = *(factory.find_worker_by_id(2));
    w2.receiver_preferences_.add_receiver(&(*factory.find_storehouse_by_id(1)));
    w2.receiver_preferences_.add_receiver(&(*factory.find_storehouse_by_id(2)));

    // -----------------------------------------------------------------------

    std::vector<std::string> expected_report_lines{
            "",
            "== LOADING RAMPS ==",
            "",
            "LOADING RAMP #1",
            "  Delivery interval: 1",
            "  Receivers:",
            "    worker #1",
            "",
            "LOADING RAMP #2",
            "  Delivery interval: 2",
            "  Receivers:",
            "    worker #1",
            "    worker #2",
            "",
            "",
            "== WORKERS ==",
            "",
            "WORKER #1",
            "  Processing time: 1",
            "  Queue type: FIFO",
            "  Receivers:",
            "    storehouse #1",
            "",
            "WORKER #2",
            "  Processing time: 2",
            "  Queue type: LIFO",
            "  Receivers:",
            "    storehouse #1",
            "    storehouse #2",
            "",
            "",
            "== STOREHOUSES ==",
            "",
            "STOREHOUSE #1",
            "",
            "STOREHOUSE #2",
            "",
    };

    perform_structure_report_check(factory, expected_report_lines);
}

TEST(ReportsTest, TurnReportNoPackages) {
    // Utwórz fabrykę.
    Factory factory;

    factory.add_ramp(Ramp(1, 10));
    factory.add_worker(Worker(1, 1, std::make_unique<PackageQueue>(PackageQueueType::FIFO)));
    factory.add_storehouse(Storehouse(1));

    Ramp& r = *(factory.find_ramp_by_id(1));
    r.receiver_preferences_.add_receiver(&(*factory.find_worker_by_id(1)));

    Worker& w = *(factory.find_worker_by_id(1));
    w.receiver_preferences_.add_receiver(&(*factory.find_storehouse_by_id(1)));

    // Ustaw warunki początkowe symulacji.
    Time t = 1;

    // -----------------------------------------------------------------------

    std::vector<std::string> expected_report_lines{
            "=== [ Turn: " + std::to_string(t) + " ] ===",
            "",
            "== WORKERS ==",
            "",
            "WORKER #1",
            "  PBuffer: (empty)",
            "  Queue: (empty)",
            "  SBuffer: (empty)",
            "",
            "",
            "== STOREHOUSES ==",
            "",
            "STOREHOUSE #1",
            "  Stock: (empty)",
            "",
    };

    perform_turn_report_check(factory, t, expected_report_lines);
}

TEST(ReportsTest, TurnReportPackageInProcessingBuffer) {
    // Utwórz fabrykę.
    Factory factory;

    factory.add_ramp(Ramp(1, 10));
    factory.add_worker(Worker(1, 2, std::make_unique<PackageQueue>(PackageQueueType::FIFO)));
    factory.add_storehouse(Storehouse(1));

    Ramp& r = *(factory.find_ramp_by_id(1));
    r.receiver_preferences_.add_receiver(&(*factory.find_worker_by_id(1)));

    Worker& w = *(factory.find_worker_by_id(1));
    w.receiver_preferences_.add_receiver(&(*factory.find_storehouse_by_id(1)));

    // Ustaw warunki początkowe symulacji.
    Time t = 1;
    r.deliver_goods(t);
    r.send_package();
    w.do_work(t);

    // -----------------------------------------------------------------------

    std::vector<std::string> expected_report_lines{
            "=== [ Turn: " + std::to_string(t) + " ] ===",
            "",
            "== WORKERS ==",
            "",
            "WORKER #1",
            "  PBuffer: #1 (pt = 1)",
            "  Queue: (empty)",
            "  SBuffer: (empty)",
            "",
            "",
            "== STOREHOUSES ==",
            "",
            "STOREHOUSE #1",
            "  Stock: (empty)",
            "",
    };

    perform_turn_report_check(factory, t, expected_report_lines);
}

TEST(ReportsTest, TurnReportPackageInQueue) {
    // Utwórz fabrykę.
    Factory factory;

    factory.add_ramp(Ramp(1, 10));
    factory.add_worker(Worker(1, 2, std::make_unique<PackageQueue>(PackageQueueType::FIFO)));
    factory.add_storehouse(Storehouse(1));

    Ramp& r = *(factory.find_ramp_by_id(1));
    r.receiver_preferences_.add_receiver(&(*factory.find_worker_by_id(1)));

    Worker& w = *(factory.find_worker_by_id(1));
    w.receiver_preferences_.add_receiver(&(*factory.find_storehouse_by_id(1)));

    // Ustaw warunki początkowe symulacji.
    Time t = 1;
    r.deliver_goods(t);
    r.send_package();

    // -----------------------------------------------------------------------

    std::vector<std::string> expected_report_lines{
            "=== [ Turn: " + std::to_string(t) + " ] ===",
            "",
            "== WORKERS ==",
            "",
            "WORKER #1",
            "  PBuffer: (empty)",
            "  Queue: #1",
            "  SBuffer: (empty)",
            "",
            "",
            "== STOREHOUSES ==",
            "",
            "STOREHOUSE #1",
            "  Stock: (empty)",
            "",
    };

    perform_turn_report_check(factory, t, expected_report_lines);
}

TEST(ReportsTest, TurnReportPackageInSendingBuffer) {
    // Utwórz fabrykę.
    Factory factory;

    factory.add_ramp(Ramp(1, 10));
    factory.add_worker(Worker(1, 1, std::make_unique<PackageQueue>(PackageQueueType::FIFO)));
    factory.add_storehouse(Storehouse(1));

    Ramp& r = *(factory.find_ramp_by_id(1));
    r.receiver_preferences_.add_receiver(&(*factory.find_worker_by_id(1)));

    Worker& w = *(factory.find_worker_by_id(1));
    w.receiver_preferences_.add_receiver(&(*factory.find_storehouse_by_id(1)));

    // Ustaw warunki początkowe symulacji.
    Time t = 1;
    r.deliver_goods(t);
    r.send_package();
    w.do_work(t);

    // -----------------------------------------------------------------------

    std::vector<std::string> expected_report_lines{
            "=== [ Turn: " + std::to_string(t) + " ] ===",
            "",
            "== WORKERS ==",
            "",
            "WORKER #1",
            "  PBuffer: (empty)",
            "  Queue: (empty)",
            "  SBuffer: #1",
            "",
            "",
            "== STOREHOUSES ==",
            "",
            "STOREHOUSE #1",
            "  Stock: (empty)",
            "",
    };

    perform_turn_report_check(factory, t, expected_report_lines);
}

TEST(ReportsTest, TurnReportPackageInStock) {
    // Utwórz fabrykę.
    Factory factory;

    factory.add_ramp(Ramp(1, 10));
    factory.add_storehouse(Storehouse(1));

    Ramp& r = *(factory.find_ramp_by_id(1));
    r.receiver_preferences_.add_receiver(&(*factory.find_storehouse_by_id(1)));

    // Ustaw warunki początkowe symulacji.
    Time t = 1;
    r.deliver_goods(t);
    r.send_package();

    // -----------------------------------------------------------------------

    std::vector<std::string> expected_report_lines{
            "=== [ Turn: " + std::to_string(t) + " ] ===",
            "",
            "== WORKERS ==",
            "",
            "",
            "== STOREHOUSES ==",
            "",
            "STOREHOUSE #1",
            "  Stock: #1",
            "",
    };

    perform_turn_report_check(factory, t, expected_report_lines);
}

TEST(FactoryIOTest, LoadAndSaveTest) {
    std::string r1 = "LOADING_RAMP id=1 delivery-interval=3";
    std::string r2 = "LOADING_RAMP id=2 delivery-interval=2";
    std::string w1 = "WORKER id=1 processing-time=2 queue-type=FIFO";
    std::string w2 = "WORKER id=2 processing-time=1 queue-type=LIFO";
    std::string s1 = "STOREHOUSE id=1";
    std::string l1 = "LINK src=ramp-1 dest=worker-1";
    std::string l2 = "LINK src=ramp-2 dest=worker-1";
    std::string l3 = "LINK src=ramp-2 dest=worker-2";
    std::string l4 = "LINK src=worker-1 dest=worker-1";
    std::string l5 = "LINK src=worker-1 dest=worker-2";
    std::string l6 = "LINK src=worker-2 dest=store-1";

    std::set<std::string> input_set = {r1, r2, w1, w2, s1, l1, l2, l3, l4, l5, l6};

    std::vector<std::string> input_lines{
            "; == LOADING RAMPS ==",
            "",
            r1,
            r2,
            "",
            "; == WORKERS ==",
            "",
            w1,
            w2,
            "",
            "; == STOREHOUSES ==",
            "",
            s1,
            "",
            "; == LINKS ==",
            "",
            l1,
            "",
            l2,
            l3,
            "",
            l4,
            l5,
            "",
            l6,
    };

    // ignore empty lines, ignore comments ("; ...")
    // resulting set must be empty

    std::string factoryStructureStr = std::accumulate(input_lines.begin(), input_lines.end(), std::string(""),
                                                      [](std::string accu, std::string line) {
                                                          return accu + line + "\n";
                                                      });

    std::istringstream iss(factoryStructureStr);
    Factory factory;
    try {
        factory = load_factory_structure(iss);
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        FAIL();
    } catch (...) {
        std::cerr << "Unknown error..." << std::endl;
        FAIL();
    }

    std::ostringstream oss;
    save_factory_structure(factory, oss);

    std::vector<std::string> output_lines;
    //
    std::string structure_str = oss.str();

    iss.str(structure_str);
    iss.clear();
    std::string line;
    while (std::getline(iss, line, '\n')) {
        bool is_blank_line = line.empty() || isblank(line[0]);
        bool is_comment_line = !line.empty() && line[0] == ';';
        if (is_blank_line || is_comment_line) {
            continue;
        }

        output_lines.push_back(line);
    }

    std::set<std::string> output_set(output_lines.begin(), output_lines.end());
    ASSERT_EQ(output_set.size(), output_lines.size()) << "Duplicated lines in the output.";

    EXPECT_THAT(output_set, ContainerEq(input_set));

    // Elementy w pliku występującą w następującej kolejności: LOADING_RAMP, WORKER, STOREHOUSE, LINK.

    auto first_occurence = [&output_lines](std::string label) {
        return std::find_if(output_lines.begin(), output_lines.end(),
                            [label](const std::string s) { return s.rfind(label, 0) == 0; });
    };

    auto first_ramp_it = first_occurence("LOADING_RAMP");
    auto first_worker_it = first_occurence("WORKER");
    auto first_storehouse_it = first_occurence("STOREHOUSE");
    auto first_link_it = first_occurence("LINK");

    ASSERT_LT(first_ramp_it, first_worker_it);
    ASSERT_LT(first_worker_it, first_storehouse_it);
    ASSERT_LT(first_storehouse_it, first_link_it);
}
